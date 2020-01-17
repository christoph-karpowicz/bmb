#include "persist.h"

/* 
persist_dispatch: 
1. recevies persistence request structs,
2. checks if the request contains all the necessary data,
3. casts the request data to an appropriate data type,
4. calls the right persistence function based on the received request type,
5. returns a persistence response struct.

Persistence request types:
ADD_QUEUE       1
REMOVE_QUEUE    2
READ_QUEUE      3
GET_QUEUE_LIST  4
ADD_NODE        5
REMOVE_NODE     6
READ_NODE       7
GET_NEXT_ID     8
*/
struct persist_response persist_dispatch(struct persist_request req)
{
	struct persist_response res = {NULL, true, NULL};
	char path[50] = "./data";

	const bool queueNameNeeded = 
		req.type == PERSIST_ADD_QUEUE || 
		req.type == PERSIST_READ_QUEUE || 
		req.type == PERSIST_REMOVE_QUEUE ||
		req.type == PERSIST_ADD_NODE ||
		req.type == PERSIST_REMOVE_NODE ||
		req.type == PERSIST_READ_NODE;
	if (queueNameNeeded && !req.queueName) {
		res.errorMessage = create_error_message("Error: ", "Queue name needed.");
		res.success = false;
		goto resReturn;
	}

	if (req.queueName) {
		strcat(path, "/");
		strcat(path, req.queueName);
	}

	char *nodeIdStr = NULL;
	if (req.nodeId > 0) {
		nodeIdStr = int_to_string(req.nodeId);
	}

    if (req.type == PERSIST_ADD_QUEUE) {
		if (!create_queue_dir(path)) {
			res.success = false;
			res.errorMessage = create_error_message("Error creating directory: ", strerror(errno));
		}
	}
    else if (req.type == PERSIST_REMOVE_QUEUE) {
		if (!persist_remove_queue(path)) {
			res.success = false;
			res.errorMessage = create_error_message("Error removing queue: ", strerror(errno));
		}
	}
	else if (req.type == PERSIST_READ_QUEUE) {
		const size_t queueLength = get_queue_length(path);
		int *queue = NULL;
		
		if (!(queue = read_queue(path))) {
			res.success = false;
			res.errorMessage = create_error_message("Error reading queue: ", strerror(errno));
		}
		else {
			res.data = queue;
		}
	}
	else if (req.type == PERSIST_GET_QUEUE_LIST) {
		void **queues = NULL;
		if (!(queues = get_queue_list())) {
			res.success = false;
			res.errorMessage = create_error_message("Error getting queue list: ", strerror(errno));
		}
		else {
			res.data = queues;
		}
	}
	else if (req.type == PERSIST_ADD_NODE) {
		if (!create_node_file(path, nodeIdStr, (char *) req.data)) {
			res.success = false;
			res.errorMessage = create_error_message("Error creating file: ", strerror(errno));
		}
	}
    else if (req.type == PERSIST_REMOVE_NODE) {
		if (!remove_node(path, nodeIdStr)) {
			res.success = false;
			res.errorMessage = create_error_message("Error removing file: ", strerror(errno));
		}
	}
	else if (req.type == PERSIST_READ_NODE) {
		char *nodeContent = read_node(path, nodeIdStr);
		if (nodeContent) {
			res.data = nodeContent;
		}
		else {
			res.success = false;
			res.errorMessage = create_error_message("Error reading node: ", strerror(errno));
		}
	}
	else if (req.type == PERSIST_GET_NEXT_ID) {
		unsigned int *nextId = get_next_node_id(path);
		if (nextId) {
			res.data = nextId;
		}
		else {
			res.success = false;
			res.errorMessage = create_error_message("Error getting next node id: ", strerror(errno));
		}
	}
	else {
		res.success = false;
		res.errorMessage = create_error_message("Error: ", "Persistence request type not recognized.");
	}

	mem_free(nodeIdStr);

	resReturn:
	return res;
}

static DIR *check_dir(const char *path)
{
	DIR *dir;
	if ((dir = opendir(path)) != NULL) {
		return dir;
	}

	return NULL;
}

static char *create_error_message(const char *part1, const char *part2)
{
	char *errMsg = (char *) mem_alloc(strlen(part1) + strlen(part2) + 1);
	strcpy(errMsg, part1);
	strcat(errMsg, part2);
	return errMsg;
}

/* 
create_node_file creates a new file in a queue directory, which:
1. is named with the node's id,
2. contains the nodes message.
*/
static bool create_node_file(const char *path, const char *name, const char *content)
{
	FILE *fp = NULL;
	char filePath[50];
	strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, name);

	fp = fopen(filePath ,"w");
	if (fp) {
		fprintf(fp, "%s", content);
		fclose(fp);
		return true;
	}

	return false;
}

/*
create_queue_dir creates a new directory in the 'data' directory, which:
1. is named with the queue's name,
2. will contain created by create_node_file function.
*/
static bool create_queue_dir(const char *path)
{
    bool success 	= true;
    DIR *dir 		= opendir(path);
    
    if (dir) {
        closedir(dir);
    } else if (ENOENT == errno) {
		// printf("%s\n", path);
        int result = mkdir(path, 0777);
		if (result == -1) {
			success = false;
		}
    } else {
        success = false;
    }

    return success;
}

/* 
get_next_node_id gets the given queue's last node file name 
	and returns an id for the next node by adding 1 to it.
*/
static unsigned int *get_next_node_id(const char *path)
{
	const size_t queueLength = get_queue_length(path);
	int *queue = NULL;
	
	if (queue = read_queue(path)) {
		const int queueSize = queue[0];
		unsigned int *nextId = (unsigned int *) mem_alloc(sizeof(unsigned int));
		if (queueSize > 0) {
			*nextId = queue[queueSize] + 1;
		} else {
			*nextId = 1;
		}
		mem_free(queue);
		return nextId;
	}

	return NULL;
}

static size_t get_queue_length(const char *path)
{
	DIR *dir = check_dir(path);
	if (!dir) return 0;

	struct dirent *ent;
	size_t count = 0;

	while ((ent = readdir(dir)) != NULL) {
		if (atoi(ent->d_name) != 0) {
			count++;
		}
	}
	closedir(dir);

	return count;
}

/*
get_queue_list returns a dynamically allocated array
	with names of all the queues (directories in the 'data' folder).
*/
static void **get_queue_list()
{
	DIR *dir = check_dir("./data");
	if (!dir) return NULL;

	struct dirent *ent;
	size_t count = 0;
	void **queueList = (void **) mem_alloc(sizeof(size_t *));

	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
			// printf("- %s\n", ent->d_name);
			queueList = (void **) realloc(queueList, sizeof(size_t) + (sizeof(char *) * (count + 1)));
			
			char *queueName = (char *) mem_alloc(strlen(ent->d_name) + 1);
			strcpy(queueName, ent->d_name);
			queueList[++count] = queueName;
		}
	}
	closedir(dir);

	size_t *size = (void *) mem_alloc(sizeof(size_t));
	*size = count;
	queueList[0] = size;

	return queueList;
}

/*
read_node reads and returns the nodes content (message) from a node file.
*/
static char *read_node(const char *path, const char *name)
{
	FILE *fp = NULL;
	char filePath[50];
	strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, name);

	fp = fopen(filePath ,"r");
	if (fp) {
		
		size_t i = 0;
		size_t buff_len = 0;
		char *nodeContent = (char *) mem_alloc(buff_len);
		char c;

		while ((c = fgetc(fp)) != EOF) {
			buff_len++;
			char *tmp = (char *) realloc(nodeContent, buff_len + 1);
			nodeContent = tmp;
			nodeContent[i] = (char) c;
			i++;
			// printf("%c",c);
	    }
		nodeContent[i] = '\0';
		
		fclose(fp);
		
		return nodeContent;
	}

	return NULL;
}

static bool remove_node(const char *path, const char *name)
{
	char filePath[50];
	int unlinked;
	strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, name);

	if ((unlinked = unlink(filePath)) == -1) {
		return false;
	}

	return true;
}

/*
read_queue returns a dynamically allocated array of
	node file ids from a given queue directory name.
	It's an (int *) array because node's ids are always integers.
*/
static int *read_queue(const char *path)
{
	DIR *dir = check_dir(path);
	if (!dir) return NULL;

	struct dirent *ent;
	int *nodeList = (int *) mem_alloc(sizeof(int));
	size_t count = 0;

	while ((ent = readdir(dir)) != NULL) {
		if (atoi(ent->d_name) != 0) {
			nodeList = (int *) realloc(nodeList, sizeof(int) + (sizeof(int) * (count + 1)));
			nodeList[++count] = atoi(ent->d_name);
		}
	}
	closedir(dir);

	int *nodeListSlice = nodeList + 1;
	if (count > 0)
		nodeListSlice = sort_int_array_asc(nodeListSlice, count);

	// for (size_t i = 0; i < count; i++) {
	// 	printf("%d\n", nodeListSlice[i]);
	// }

	nodeList[0] = count;

	return nodeList;
}

static bool persist_remove_queue(const char *path)
{
	DIR *dir = check_dir(path);
	if (!dir) return NULL;

	int *nodeFileArr = read_queue(path);
	const size_t queueLength = get_queue_length(path);

	for (size_t i = 1; i <= queueLength; i++) {
		char *nodeIdStr = int_to_string(nodeFileArr[i]);
		bool nodeRemoved = remove_node(path, nodeIdStr);

		mem_free(nodeIdStr);
		
		if (!nodeRemoved) {
			return false;
		}
	}
	
	mem_free(nodeFileArr);

	if (rmdir(path) != 0)
		return false;

	return true;
}
