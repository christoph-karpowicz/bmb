#include "persist.h"

/* 
	Persistence request types:
	1 - add queue
	2 - remove queue
	3 - add node
	4 - remove node
*/
struct persist_response persist_dispatch(struct persist_request req)
{
	struct persist_response res = {NULL, true};

	char path[50] 	= "../data/";
	strcat(path, req.queueName);

    if (req.type == 1) {
		if (!create_queue_dir(path)) {
			fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
			exit(1);
		}
	}
    else if (req.type == 2) {

	}
	else if (req.type == 3) {
		int length = (int)((ceil(log10(req.nodeIndex))+1)*sizeof(char));
		char nodeIndexStr[length];
		sprintf(nodeIndexStr, "%d", req.nodeIndex);

		if (!create_node_file(path, nodeIndexStr, "testestes sadsadsad\nsdads 1111111111111")) {
			fprintf(stderr, "Error creating file: %s\n", strerror(errno));
			exit(1);
		}
	}
    else if (req.type == 4) {

	}
	else {
		res.success = false;
		res.errorMessage = "Persist request type not recognized.";
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
	fprintf(fp, "%s", content);
	fclose(fp);
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

static unsigned int list_queue_dir(const char *path)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			printf ("%s\n", ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
}

void persist_destruct(Persist *persist)
{
    free(persist);
}