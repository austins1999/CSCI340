
// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2019
// server.h header file
// Homework 2
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include "server.h"

// ------------------------------------
// Function prototype that creates a socket and 
// then binds it to the specified port_number 
// for incoming client connections
// 
//
// Arguments:	port_number = port number the server 
//				socket will be bound to.
//
// Return:      Socket file descriptor (or -1 on failure)
//

int bind_port( unsigned int port_number ) {

	// -------------------------------------
	// NOTHING TODO HERE :)
	// -------------------------------------
	// Please do not modify

	int socket_fd;
	int set_option = 1;

    struct sockaddr_in server_address;
     
    socket_fd = socket( AF_INET, SOCK_STREAM, 0 );

    setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set_option, sizeof( set_option ) );

    if (socket_fd < 0) return FAIL;

    bzero( (char *) &server_address, sizeof(server_address) );

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons( port_number );

    if ( bind( socket_fd, (struct sockaddr *) &server_address, sizeof(server_address) ) == 0 ) {

    	return socket_fd;

    } else {

    	return FAIL;

    }

} // end bind_port function

// ------------------------------------
// Function prototype that accepts a client
// socket connection
// 
//
// Arguments:	server file descriptor
//
// Return:      Termination status of client
//				( 0 = No Errors, -1 = Error )
//
int accept_client( int server_socket_fd ) {

	int exit_status = OK;

	int client_socket_fd = -1;

	socklen_t client_length; 

	struct sockaddr_in client_address;

	char request[2048];

	// My variable declarations: start
	int pid;
	int i;
	char* URL;
	char* endChar;
	int endLoc;
	int lineBreakCount;
	// end

	client_length = sizeof( client_address );

    client_socket_fd = accept( server_socket_fd, (struct sockaddr *) &client_address, &client_length );
		
	// -------------------------------------
	// TODO:
	// -------------------------------------
	// Modify code to fork a child process
	// -------------------------------------
	

	if ( client_socket_fd >= 0 ) {

		// start

		pid = fork();

		if( pid == 0 ) {	//child; process html
	
			bzero( request, 2048 );
			
			read( client_socket_fd, request, 2047 );
		
			if ( DEBUG ) printf("Here is the http message:\n%s\n\n", request );
		
			// -------------------------------------
			// TODO:
			// -------------------------------------
			// Generate the correct http response when a GET or POST method is sent
			// from the client to the server.
			// 
			// In general, you will parse the request character array to:
			// 1) Determine if a GET or POST method was used
			// 2) Then retrieve the key/value pairs (see below)
			// -------------------------------------

			
			char entity_body[500];
			bzero( entity_body, 500 );
			strcat(entity_body, "<html><body><h1>");

			if ( request[0] == 'P' ) { //POST

				printf("POST command detected! Now processing POST.\n");
				strcat(entity_body, "POST Operation</h1><table cellpadding=5 cellspacing=5 border=1><tr><th>Key</th><th>Value</th></tr>");
			
			//1 - PARSE REQUEST

				//Find body starting location
				//Start from beginning and count cr/lf combos
				URL = request;
				lineBreakCount = 0;
				while ( lineBreakCount < 11 ) {
					URL += 2;
					URL = strchr(URL, '\r');
					lineBreakCount++;
				}

				URL += 2; //Filters out remaining cr/lf characters


			//2 COUNT KEY/VALUE PAIRS

				i = 0;
				strcat(entity_body, "<tr><td>");

				//Find first key/value pair; this is a special case, since it
				//  is not preceeded by an '&'
				while( URL[i] != '=' ) {
					strncat(entity_body, &URL[i], 1);
					i++;
				}
				i++;
				strcat(entity_body, "</td><td>");
				while( URL[i] != '&' && URL[i] != '\0' ) {
					strncat(entity_body, &URL[i], 1);
					i++;
				}
				strcat(entity_body, "</td></tr>");

				//Find rest of the key/value pairs
				while ( URL[i] != '\0' ) {
					if ( URL[i] == '&' ) {
						i++;
						strcat(entity_body, "<tr><td>");
						while( URL[i] != '=' ) {
							strncat(entity_body, &URL[i], 1);
							i++;
						}
						strcat(entity_body, "</td><td>");
						i++;
						while( URL[i] != '&' && URL[i] != '\0') {
							strncat(entity_body, &URL[i], 1);
							i++;
						}
						strcat(entity_body, "</td></tr>");
					}
					else {
						i++;
					}
				}
			}



			else { //GET

				printf("GET command detected! Now processing GET.\n");
				strcat(entity_body, "GET Operation</h1><table cellpadding=5 cellspacing=5 border=1><tr><th>Key</th><th>Value</th></tr>");

			//1 - PARSE REQUEST

				//Isolates URL and populates entity body
				URL = strchr( request, '/' );
				endChar = strchr( URL, ' ' );
				endLoc = endChar - URL;
				i = 0;
				while ( i < endLoc ) {
					if ( URL[i] == '&' || URL[i] == '?' ) {
						i++;
						strcat(entity_body, "<tr><td>");
						while( URL[i] != '=' ) {
							strncat(entity_body, &URL[i], 1);
							i++;
						}
						strcat(entity_body, "</td><td>");
						i++;
						while( URL[i] != '&' && URL[i] != ' ') {
							strncat(entity_body, &URL[i], 1);
							i++;
						}
						strcat(entity_body, "</td></tr>");
					}
					else {
						i++;
					}
				}
				printf("\n");

			}



			strcat(entity_body, "</table></body></html>\0");
			//char* entity_body = "<html><body><h1>GET Operation</h1><table cellpadding=5 cellspacing=5 border=1><tr><th>Key</th><th>Value</th></tr></table></body></html>";

			char response[512];
			sprintf( response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s", (int)strlen( entity_body ), entity_body );
		
			if ( DEBUG ) printf( "%s\n", response );
		
    		write( client_socket_fd, response, strlen( response ) );
		
			close( client_socket_fd );		

			exit(0);

		}
		
		else if ( pid > 0 ) {

			close( client_socket_fd );	
		
		}	
		
		
	} else {
		
		exit_status = FAIL;
		
	}
	
	if ( DEBUG ) printf("Exit status = %d\n", exit_status );
	
	return exit_status;
	
} // end accept_client function
