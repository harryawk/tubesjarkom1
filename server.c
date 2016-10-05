#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

char *cmd = "free";

#define BUFSIZE 128
char buf[BUFSIZE];
char memtotal[BUFSIZE];
char memfree[BUFSIZE];
char memused[BUFSIZE];
char hostname[HOST_NAME_MAX];

char* parse_command(int tipe) { 
   // tipe 1 = total memory
   // tipe 2 = used memory
   // tipe 3 = free memory

// masih bug
  FILE *fp;

  if ((fp = popen(cmd, "r")) == NULL) {
      printf("Error opening pipe!\n");
      return -1;
  }
      
      // Do whatever you want here...
  bzero(memtotal, BUFSIZE);
  bzero(memfree, BUFSIZE);
  bzero(memused, BUFSIZE);
  int iterasi = 1;
  while(fgets(buf, BUFSIZE, fp) != NULL) {

      if (iterasi == 2) {
          int i = 0;
          while (buf[i] != ' ') {
              i++;
          } // end of "Mem:"
          while (buf[i] == ' ') {
              i++;
          } // total memory
          int j = 0;
          while (buf[i] != ' ') {
              if (tipe == 1) {
               memtotal[j] = buf[i];
              }
              i++;
              j++;
          } // end of total memory
          j = 0;
          while (buf[i] == ' ') {
              i++;
          } // used memory
          while (buf[i] != ' ') {
              if (tipe == 2) {
               memused[j] = buf[i];
              }
              i++;
              j++;
          } // end of used memory
          j = 0;
          while (buf[i] == ' ') {
              i++;
          } // free memory
          while (buf[i] != ' ') {
               if (tipe == 3) {
               memfree[j] = buf[i];
              }
              i++;
              j++;
          } // end of free memory
          if (tipe == 1) {
               return memtotal;
            } else if (tipe == 2) {
               return memused;
            } else if (tipe == 3) {
               return memfree;
            }
      }
      iterasi++;
      
      
  }

  if(pclose(fp))  {
      printf("Command not found or exited with error status\n");
      return -1;
  }
}

void doprocessing (int communication_fd) {
   int n;
   unsigned long long value;
   char str[100];
   char buffer[1024];
   
   write(communication_fd, "# munin node at ", 17);
   gethostname(hostname, HOST_NAME_MAX);
   write(communication_fd, hostname, HOST_NAME_MAX);
   write(communication_fd, "\n", 1);

   bzero(buffer, 1024);
   strcpy(buffer, "list ");
   strcat(buffer, hostname);
   while(1) {
    // Mengisi buffer untuk komunikasi dengan 0
      bzero(str, 100);
     // Membaca request dari client
     read(communication_fd,str,100);     
     printf("task : %s\n",str);
     if (strncmp(str,"cap",strlen("cap")) == 0) { // cap
      strcpy(str,"cap multigraph dirtyconfig\n");
      printf("HASIL : %s\n",str);
     } 
     else if (strncmp(str,"nodes",strlen("nodes")) == 0) { // nodes
      strcpy(str,hostname);
      printf("HASIL : %s\n",str);
      write(communication_fd, str, strlen(str)+1);
      write(communication_fd, "\n", strlen("\n")+1);
      write(communication_fd, ".\n", 3);
      continue;
     } 
     else if (strncmp(str,buffer,strlen(buffer)) == 0 || strncmp(str,"list", strlen("list")) == 0) { // list MyComputer
      strcpy(str, "memory\n");
      printf("HASIL : %s\n",str);
     } 
     else if (strncmp(str,"config memory",strlen("config memory")) == 0) { // config memory
      write(communication_fd, "graph_args --base 1024 -l 0 --upper-limit ", strlen("graph_args --base 1024 -l 0 --upper-limit "));
      strcpy(str, parse_command(1));
      value = atoi(str)*1024;
      sprintf(str, "%llu", value);
      printf("HASIL : %s\n",str);
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n"));
      write(communication_fd, "graph_vlabel Bytes\n", strlen("graph_vlabel Bytes\n"));
      write(communication_fd, "graph_title Memory usage\n", strlen("graph_title Memory usage\n"));
      write(communication_fd, "graph_category system\n", strlen("graph_category system\n"));
      write(communication_fd, "graph_info This graph shows this machine memory.\n", strlen("graph_info This graph shows this machine memory.\n"));
      write(communication_fd, "graph_order used free\n", strlen("graph_order used free\n"));
      write(communication_fd, "used.label used\n", strlen("used.label used\n"));
      write(communication_fd, "used.draw STACK\n", strlen("used.draw STACK\n"));
      write(communication_fd, "used.info Used memory.\n", strlen("used.info Used memory.\n"));
      write(communication_fd, "free.label free\n", strlen("free.label free\n"));
      write(communication_fd, "free.draw STACK\n", strlen("free.draw STACK\n"));
      write(communication_fd, "free.info Free memory.\n", strlen("free.info Free memory.\n"));
      write(communication_fd, ".\n", 2);
      continue;
     } 
     else if (strncmp(str,"fetch memory",strlen("fetch memory")) == 0) { // fetch memory
      write(communication_fd, "used.value ", strlen("used.value "));
      strcpy(str, parse_command(2));
      value = atoi(str)*1024;
      sprintf(str, "%llu", value);
      printf("HASIL : %s\n",str);
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n")); // used.value 120933018402
      write(communication_fd, "free.value ", strlen("free.value "));
      strcpy(str, parse_command(3));
      printf("HASIL : %s\n",str);
      value = atoi(str)*1024;
      sprintf(str, "%llu", value);
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n")); // free.value 1287498127
      write(communication_fd, ".\n", 2);
      continue;
     } 
     else if (strncmp(str,"version",strlen("version")) == 0) { // version
      strcpy(str, "TropicalIsland v1.0\n");
      printf("HASIL : %s\n",str);
     } 
     else if (strncmp(str,"quit", strlen("quit")) == 0) { // quit
      strcpy(str, "\n");
      write(communication_fd, str, strlen(str));
      break;
    }
    else { // otherwise
      strcpy(str, "# Unknown command. Try cap, list, nodes, config, fetch, version or quit\n");
    }
    // Mengirim response ke client
    write(communication_fd, str, strlen(str));
   }
}

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n, pid;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 4949;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   /* Now start listening for the clients, here
      * process will go in sleep mode and will wait
      * for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      /* Create child process */
      pid = fork();
		
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
         /* This is the client process */
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
		
   } /* end of while */
}
