app: src/app.c src/server.c src/s_thread.c src/queue.c src/queue_node.c src/request.c
	cc src/app.c src/server.c src/s_thread.c src/queue.c src/queue_node.c src/request.c -o app -pthread -lnsl