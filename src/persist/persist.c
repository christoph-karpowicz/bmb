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
	struct persist_response res = {NULL, true};
	char path[50] 				= "../data/";
	strcat(path, req.queueName);

	const size_t queueLength = get_queue_length(path);
	int *arr = read_queue(path);
	for (size_t i = 0; i < queueLength; i++) {
		printf("%d\n", arr[i]);
	}

	printf("\n\n%s\n", read_node(path, "11"));

    if (req.type == PERSIST_ADD_QUEUE) {
		if (!create_queue_dir(path)) {
			fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
			exit(1);
		}
	}
    else if (req.type == PERSIST_REMOVE_QUEUE) {

	}
	else if (req.type == PERSIST_READ_QUEUE) {

	}
	else if (req.type == PERSIST_ADD_NODE) {
		int length = (int)((ceil(log10(req.nodeIndex)) + 1) * sizeof(char));
		char nodeIndexStr[length];
		sprintf(nodeIndexStr, "%d", req.nodeIndex);

		if (!create_node_file(path, nodeIndexStr, "testestes sadsadsad\nsdads 1111111111111")) {
			fprintf(stderr, "Error creating file: %s\n", strerror(errno));
			exit(1);
		}
	}
    else if (req.type == PERSIST_REMOVE_NODE) {

	}
	else if (req.type == PERSIST_READ_NODE) {

	}
	else {
		res.success = false;
		res.errorMessage = "Persistence request type not recognized.";
	}

	return res;
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
	DIR *dir;
	struct dirent *ent;
	size_t count = 0;
	if ((dir = opendir (path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (atoi(ent->d_name) != 0) {
				count++;
			}
		}
		closedir (dir);
	} else {
		perror ("");
		// return EXIT_FAILURE;
	}

	return count;
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

		while ((c=fgetc(fp)) != EOF) {
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

static int *read_queue(const char *path)
{
	DIR *dir;
	struct dirent *ent;
	int *nodeFileArr = NULL;
	size_t count = 0;
	if ((dir = opendir (path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (nodeFileArr == NULL) {
				nodeFileArr = (int *) malloc(sizeof(int));
			}
			else {
				nodeFileArr = (int *) realloc(nodeFileArr, sizeof(int) * (count + 1));
			}

			if (atoi(ent->d_name) != 0) {
				nodeFileArr[count++] = atoi(ent->d_name);
				printf ("%d\n", atoi(ent->d_name));
			}
		}
		closedir (dir);
	} else {
		perror ("");
		// return EXIT_FAILURE;
	}

	return nodeFileArr;
}

void persist_destruct(Persist *persist)
{
    free(persist);
}