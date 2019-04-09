# server
/**
 * Create a chat-server.  Things to consider:
 * 1) Create a client list structure or array.  Things to consider having is
 *    some way to mark a client as being dead before it is removed.  Anytime a
 *    write fails, that client should be disconnected, but it may not be easy to
 *    remove the client immediately. This can be local to main, but will then
 *    need to be passed to any functions that need access to it.
 */
 /*
 * 2) Consider making a function that does the accept and inserts the new file
 *    descriptor into the client list.
 */  
/*
 3) Consider making a function to clean up dead connections from the
 *    connection list, or do a cleanup pass before entering back into select.
 * 
 *    Both adding and removing connections should update a saved read fd_set
 *    which is copied to a working fd_set before select. Both should also re-
 *    compute the highest working file-descriptor number (+1) for selects first
 *    parameter.
 */
 /*
 * 4) Consider making a function to broadcast the received message to all the
 *    currently connected clients, but not to the originator of the message.
 *    If any of the writes should fail, that client should be marked dead or
 *    removed from the client list.
 */
 /*
 * 5) The main function should create and bind a streaming internet socket on to
 *    listen for connections on your assigned port.  It should initialize the
 *    client list and initialize a saved fd_set with just the socket descriptor.
 *    It should then enter an infinite loop and use select to wait for read
 *    events.  If the read event is for the listening socket it should accept
 *    the connection, otherwise it is a client message that should be broadcast
 *    to all other clients.
 */
 


