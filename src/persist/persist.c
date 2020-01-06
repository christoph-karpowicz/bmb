#include "persist.h"

/* 
	Persistence request types:
	1 - add queue
	2 - remove queue
	3 - load queue
	4 - add node
	5 - remove node
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
		int length = (int)((ceil(log10(req.nodeId)) + 1) * sizeof(char));
		nodeIdStr = (char *) mem_alloc(length);
		sprintf(nodeIdStr, "%d", req.nodeId);
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

	for (size_t i = 0; i < queueLength; i++) {
		int length = (int)((ceil(log10(nodeFileArr[i])) + 1) * sizeof(char));
		char *nodeIdStr = (char *) mem_alloc(length);
		sprintf(nodeIdStr, "%d", nodeFileArr[i]);

		if (!remove_node(path, nodeIdStr)) {
			return false;
		}
	}
	
	mem_free(nodeFileArr);

	if (rmdir(path) != 0)
		return false;

	return true;
}
