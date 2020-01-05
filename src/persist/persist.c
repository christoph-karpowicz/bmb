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
	struct persist_response res = {NULL, true, NULL, 0};
	char path[50] = "../data/";
	strcat(path, req.queueName);

	char *nodeIdStr = NULL;
	if (req.nodeId) {
		int length = (int)((ceil(log10(req.nodeId)) + 1) * sizeof(char));
		nodeIdStr = (char *) malloc(length);
		sprintf(nodeIdStr, "%d", req.nodeId);
	}

    if (req.type == PERSIST_ADD_QUEUE) {
		if (!create_queue_dir(path)) {
			res.success = false;
			res.errorMessage = create_error_message("Error creating directory: ", strerror(errno));
		}
	}
    else if (req.type == PERSIST_REMOVE_QUEUE) {
		if (!remove_queue(path)) {
			res.success = false;
			res.errorMessage = create_error_message("Error removing queue: ", strerror(errno));
			// fprintf(stderr, "Error removing queue: %s\n", strerror(errno));
		}
	}
	else if (req.type == PERSIST_READ_QUEUE) {
		const size_t queueLength = get_queue_length(path);
		int *arr = NULL;
		
		if (!(arr = read_queue(path))) {
			res.success = false;
			res.errorMessage = create_error_message("Error reading queue: ", strerror(errno));
		}
		else {
			for (size_t i = 0; i < queueLength; i++) {
				printf("%d\n", arr[i]);
			}
			res.data = arr;
			res.arrLength = queueLength;
		}
	}
	else if (req.type == PERSIST_GET_QUEUE_LIST) {
		void **queues = NULL;
		if (!(queues = get_queue_list())) {
			res.success = false;
			res.errorMessage = create_error_message("Error getting queue list: ", strerror(errno));
		}
		else {
			for (size_t i = 1; i <= *((size_t *) queues[0]); i++) {
				// printf("%ld\n", *((size_t *) queues[0]));
				printf("%s\n", (char *) queues[i]);
			}
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
			res.errorMessage = "Failed to read node.";
		}
	}
	else {
		res.success = false;
		res.errorMessage = "Persistence request type not recognized.";
	}

	free(nodeIdStr);

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
	char *errMsg = (char *) malloc(strlen(part1) + strlen(part2) + 1);
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
	DIR *dir = check_dir("../data");
	if (!dir) return NULL;

	struct dirent *ent;
	size_t count = 0;
	void **queueList = (void **) malloc(sizeof(size_t *));

	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
			printf("- %s\n", ent->d_name);
			queueList = (void **) realloc(queueList, sizeof(size_t) + (sizeof(char *) * (count + 1)));
			
			queueList[++count] = ent->d_name;
		}
	}
	closedir(dir);

	size_t *size = (void *) malloc(sizeof(size_t));
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
		char *nodeContent = (char *) malloc(buff_len);
		char c;

		while ((c = fgetc(fp)) != EOF) {
			buff_len++;
			char *tmp = (char *) realloc(nodeContent, buff_len + 1);
			nodeContent = tmp;
			nodeContent[i] = (char) c;
			i++;
			printf("%c",c);
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
	int *nodeFileArr = NULL;
	size_t count = 0;

	while ((ent = readdir(dir)) != NULL) {
		if (atoi(ent->d_name) != 0) {
			if (nodeFileArr == NULL) {
				nodeFileArr = (int *) malloc(sizeof(int));
			}
			else {
				nodeFileArr = (int *) realloc(nodeFileArr, sizeof(int) * (count + 1));
			}

			nodeFileArr[count++] = atoi(ent->d_name);
		}
	}
	closedir(dir);

	return nodeFileArr;
}

static bool remove_queue(const char *path)
{
	DIR *dir = check_dir(path);
	if (!dir) return NULL;

	int *nodeFileArr = read_queue(path);

	const size_t queueLength = get_queue_length(path);

	for (size_t i = 0; i < queueLength; i++) {
		int length = (int)((ceil(log10(nodeFileArr[i])) + 1) * sizeof(char));
		char *nodeIdStr = (char *) malloc(length);
		sprintf(nodeIdStr, "%d", nodeFileArr[i]);

		if (!remove_node(path, nodeIdStr)) {
			return false;
		}
	}
	
	free(nodeFileArr);

	if (rmdir(path) != 0)
		return false;

	return true;
}

void persist_destruct(Persist *persist)
{
    free(persist);
}