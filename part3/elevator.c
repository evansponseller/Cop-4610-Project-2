#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cop4610");
MODULE_DESCRIPTION("pet elevator kernel module");

#define FLOOR_COUNT 5
#define MAX_PETS 5
#define MAX_WEIGHT 50
#define PROC_BUFFER_SIZE 10000
#define PROC_MODE 0444

enum elevator_state {
	ELEVATOR_OFFLINE,
	ELEVATOR_IDLE,
	ELEVATOR_LOADING,
	ELEVATOR_UP,
	ELEVATOR_DOWN
};

enum pet_type {
	CHIHUAHUA,
	PUG,
	PUGHUAHUA,
	DACHSHUND,
	PET_TYPE_COUNT
};

struct pet {
	int type;
	int start_floor;
	int destination_floor;
	int weight;
	struct list_head list;
};

struct elevator_data {
	struct mutex lock;
	struct task_struct *thread;
	struct list_head waiting[FLOOR_COUNT];
	struct list_head onboard;
	enum elevator_state state;
	int current_floor;
	int current_load;
	int waiting_count;
	int serviced_count;
	bool active;
	bool stopping;
};

static struct elevator_data elevator;
static struct proc_dir_entry *elevator_entry;

static const int pet_weights[PET_TYPE_COUNT] = {3, 14, 10, 16};
static const char pet_letters[PET_TYPE_COUNT] = {'C', 'P', 'H', 'D'};

int elevator_start(void);
int elevator_issue_request(int start_floor, int destination_floor, int type);
int elevator_stop(void);

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int, int, int);
extern int (*STUB_stop_elevator)(void);

static const char *state_name(enum elevator_state state)
{
	switch (state) {
	case ELEVATOR_IDLE:
		return "IDLE";
	case ELEVATOR_LOADING:
		return "LOADING";
	case ELEVATOR_UP:
		return "UP";
	case ELEVATOR_DOWN:
		return "DOWN";
	default:
		return "OFFLINE";
	}
}

static bool valid_request(int start_floor, int destination_floor, int type)
{
	return start_floor >= 1 && start_floor <= FLOOR_COUNT &&
		destination_floor >= 1 && destination_floor <= FLOOR_COUNT &&
		destination_floor != start_floor && type >= 0 &&
		type < PET_TYPE_COUNT;
}

static void free_pet_list(struct list_head *head)
{
	struct pet *pet;
	struct pet *next;

	list_for_each_entry_safe(pet, next, head, list) {
		list_del(&pet->list);
		kfree(pet);
	}
}

static int nearest_waiting_floor(void)
{
	int floor;
	int best_floor = -1;
	int best_distance = FLOOR_COUNT + 1;
	int distance;

	for (floor = 1; floor <= FLOOR_COUNT; floor++) {
		if (list_empty(&elevator.waiting[floor - 1]))
			continue;

		distance = abs(floor - elevator.current_floor);
		if (distance < best_distance) {
			best_distance = distance;
			best_floor = floor;
		}
	}

	return best_floor;
}

static int onboard_direction(void)
{
	struct pet *pet;

	list_for_each_entry(pet, &elevator.onboard, list) {
		if (pet->destination_floor > elevator.current_floor)
			return 1;
		if (pet->destination_floor < elevator.current_floor)
			return -1;
	}

	return 0;
}

static bool unload_at_current_floor(void)
{
	struct pet *pet;
	struct pet *next;
	bool unloaded = false;

	list_for_each_entry_safe(pet, next, &elevator.onboard, list) {
		if (pet->destination_floor != elevator.current_floor)
			continue;

		list_del(&pet->list);
		elevator.current_load -= pet->weight;
		elevator.serviced_count++;
		kfree(pet);
		unloaded = true;
	}

	return unloaded;
}

static bool load_at_current_floor(void)
{
	struct pet *pet;
	struct list_head *floor_list;
	bool loaded = false;

	floor_list = &elevator.waiting[elevator.current_floor - 1];
	while (elevator.current_load < MAX_WEIGHT &&
	       !list_empty(floor_list) &&
	       list_count_nodes(&elevator.onboard) < MAX_PETS) {
		pet = list_first_entry(floor_list, struct pet, list);
		if (list_count_nodes(&elevator.onboard) >= MAX_PETS ||
		    elevator.current_load + pet->weight > MAX_WEIGHT)
			break;

		list_move_tail(&pet->list, &elevator.onboard);
		elevator.current_load += pet->weight;
		elevator.waiting_count--;
		loaded = true;
	}

	return loaded;
}

static int onboard_count(void)
{
	struct pet *pet;
	int count = 0;

	list_for_each_entry(pet, &elevator.onboard, list)
		count++;

	return count;
}

static void elevator_step(void)
{
	int direction;
	int target;
	bool changed;

	mutex_lock(&elevator.lock);

	if (!elevator.active && !elevator.stopping) {
		elevator.state = ELEVATOR_OFFLINE;
		mutex_unlock(&elevator.lock);
		ssleep(1);
		return;
	}

	changed = unload_at_current_floor();
	if (changed) {
		elevator.state = ELEVATOR_LOADING;
		mutex_unlock(&elevator.lock);
		ssleep(1);
		return;
	}

	if (!elevator.stopping && elevator.active) {
		changed = load_at_current_floor();
		if (changed) {
			elevator.state = ELEVATOR_LOADING;
			mutex_unlock(&elevator.lock);
			ssleep(1);
			return;
		}
	}

	if (list_empty(&elevator.onboard)) {
		if (elevator.stopping) {
			elevator.active = false;
			elevator.stopping = false;
			elevator.state = ELEVATOR_OFFLINE;
			mutex_unlock(&elevator.lock);
			ssleep(1);
			return;
		}

		target = nearest_waiting_floor();
		if (target < 0) {
			elevator.state = ELEVATOR_IDLE;
			mutex_unlock(&elevator.lock);
			ssleep(1);
			return;
		}

		direction = target > elevator.current_floor ? 1 : -1;
	} else {
		direction = onboard_direction();
		if (direction == 0) {
			elevator.state = ELEVATOR_LOADING;
			mutex_unlock(&elevator.lock);
			ssleep(1);
			return;
		}
	}

	if (direction > 0 && elevator.current_floor < FLOOR_COUNT) {
		elevator.state = ELEVATOR_UP;
	} else if (direction < 0 && elevator.current_floor > 1) {
		elevator.state = ELEVATOR_DOWN;
	} else {
		elevator.state = ELEVATOR_LOADING;
	}

	mutex_unlock(&elevator.lock);
	ssleep(2);
	mutex_lock(&elevator.lock);
	if (elevator.state == ELEVATOR_UP)
		elevator.current_floor++;
	else if (elevator.state == ELEVATOR_DOWN)
		elevator.current_floor--;
	mutex_unlock(&elevator.lock);
}

static int elevator_thread(void *data)
{
	while (!kthread_should_stop())
		elevator_step();

	return 0;
}

int elevator_start(void)
{
	mutex_lock(&elevator.lock);
	if (elevator.active || elevator.stopping) {
		mutex_unlock(&elevator.lock);
		return 1;
	}

	elevator.active = true;
	elevator.state = ELEVATOR_IDLE;
	mutex_unlock(&elevator.lock);

	return 0;
}
EXPORT_SYMBOL_GPL(elevator_start);

int elevator_issue_request(int start_floor, int destination_floor, int type)
{
	struct pet *pet;

	if (!valid_request(start_floor, destination_floor, type))
		return 1;

	pet = kmalloc(sizeof(*pet), GFP_KERNEL);
	if (pet == NULL)
		return -ENOMEM;

	pet->type = type;
	pet->start_floor = start_floor;
	pet->destination_floor = destination_floor;
	pet->weight = pet_weights[type];
	INIT_LIST_HEAD(&pet->list);

	mutex_lock(&elevator.lock);
	if (elevator.stopping) {
		mutex_unlock(&elevator.lock);
		kfree(pet);
		return 1;
	}

	list_add_tail(&pet->list, &elevator.waiting[start_floor - 1]);
	elevator.waiting_count++;
	mutex_unlock(&elevator.lock);

	return 0;
}
EXPORT_SYMBOL_GPL(elevator_issue_request);

int elevator_stop(void)
{
	mutex_lock(&elevator.lock);
	if (elevator.stopping) {
		mutex_unlock(&elevator.lock);
		return 1;
	}

	elevator.active = false;
	elevator.stopping = true;
	mutex_unlock(&elevator.lock);

	return 0;
}
EXPORT_SYMBOL_GPL(elevator_stop);

static ssize_t elevator_read(struct file *file, char __user *user_buffer,
				     size_t count, loff_t *position)
{
	char *buffer;
	int length = 0;
	int floor;
	struct pet *pet;

	if (*position != 0)
		return 0;

	buffer = kmalloc(PROC_BUFFER_SIZE, GFP_KERNEL);
	if (buffer == NULL)
		return -ENOMEM;

	mutex_lock(&elevator.lock);
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Elevator state: %s\n", state_name(elevator.state));
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Current floor: %d\n", elevator.current_floor);
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Current load: %d lbs\n", elevator.current_load);
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Elevator status:");
	list_for_each_entry(pet, &elevator.onboard, list)
		length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				" %c%d", pet_letters[pet->type], pet->destination_floor);
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length, "\n");

	for (floor = FLOOR_COUNT; floor >= 1; floor--) {
		length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"[%c] Floor %d: %d", floor == elevator.current_floor ? '*' : ' ',
				floor, (int)list_count_nodes(&elevator.waiting[floor - 1]));
		list_for_each_entry(pet, &elevator.waiting[floor - 1], list)
			length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
					" %c%d", pet_letters[pet->type], pet->destination_floor);
		length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length, "\n");
	}

	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Number of pets: %d\n", onboard_count());
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Number of pets waiting: %d\n", elevator.waiting_count);
	length += scnprintf(buffer + length, PROC_BUFFER_SIZE - length,
				"Number of pets serviced: %d\n", elevator.serviced_count);
	mutex_unlock(&elevator.lock);

	length = simple_read_from_buffer(user_buffer, count, position,
					buffer, length);
	kfree(buffer);
	return length;
}

static const struct proc_ops elevator_proc_ops = {
	.proc_read = elevator_read,
};

static int __init elevator_init(void)
{
	int floor;

	mutex_init(&elevator.lock);
	INIT_LIST_HEAD(&elevator.onboard);
	for (floor = 0; floor < FLOOR_COUNT; floor++)
		INIT_LIST_HEAD(&elevator.waiting[floor]);

	elevator.state = ELEVATOR_OFFLINE;
	elevator.current_floor = 1;
	elevator.current_load = 0;
	elevator.waiting_count = 0;
	elevator.serviced_count = 0;
	elevator.active = false;
	elevator.stopping = false;

	elevator_entry = proc_create("elevator", PROC_MODE, NULL,
				     &elevator_proc_ops);
	if (elevator_entry == NULL)
		return -ENOMEM;

	elevator.thread = kthread_run(elevator_thread, NULL, "elevator");
	if (IS_ERR(elevator.thread)) {
		int error = PTR_ERR(elevator.thread);

		proc_remove(elevator_entry);
		return error;
	}

	STUB_start_elevator = elevator_start;
	STUB_issue_request = elevator_issue_request;
	STUB_stop_elevator = elevator_stop;

	return 0;
}

static void __exit elevator_exit(void)
{
	int floor;

	STUB_start_elevator = NULL;
	STUB_issue_request = NULL;
	STUB_stop_elevator = NULL;

	if (elevator.thread != NULL)
		kthread_stop(elevator.thread);

	mutex_lock(&elevator.lock);
	free_pet_list(&elevator.onboard);
	for (floor = 0; floor < FLOOR_COUNT; floor++)
		free_pet_list(&elevator.waiting[floor]);
	mutex_unlock(&elevator.lock);

	proc_remove(elevator_entry);
}

module_init(elevator_init);
module_exit(elevator_exit);
