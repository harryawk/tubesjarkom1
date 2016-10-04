#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
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
      		printf("masuk\n");
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
            printf("masuk dua\n");
		      	return memused;
		      } else if (tipe == 3) {
            printf("masuk tiga\n");
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

int main()
{
 	// buffer yang menyimpan string dari client
  char str[100];
  char buffer[1024];

  // File descriptor untuk listen and communication, diterima oleh socket
  // FD listen untuk respon dari client saat membentuk koneksi
  // FD communication untuk mengirim pesan ke client
  int listen_fd, communication_fd;

	// servaddr adalah struktur socket
  struct sockaddr_in servaddr;

  // nilai dari struktur file deskriptor yang akan diterima server saat ada client yang hendak membentuk koneksi
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Menginisialisasi struct servaddr dengan 0, sama dengan memset(&servaddr, 0, sizeof(servaddr))
  bzero((char*) &servaddr, sizeof(servaddr));

	// Assign nilai address server
	// sin_family = addressing scheme
	// sin_addr.s_addr = IP yang dapat membentuk koneksi dengan server
	// sin_port = port dimana server dapat dikontak
	// htons() mengubah nomor port ke Big Endian short integer.
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(4949);

  // Prepare untuk listen dari client di address server diatas
  if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    perror("Error on binding");
    exit(1);
  }

  // Mulai listening request koneksi, bila ada request pembentukan koneksi, listen_fd akan dikembalikan
  listen(listen_fd, 5);

  // Menunggu dan accept koneksi saat listen_fd direturn, lalu communication_fd digunakan untuk melakukan komunikasi antarclient-server
  communication_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

	write(communication_fd, "# munin node at ", 17);
  gethostname(hostname, HOST_NAME_MAX);
  write(communication_fd, hostname, HOST_NAME_MAX);
  write(communication_fd, "\n", 1);
  
  while(1)
  {
    // Mengisi buffer untuk komunikasi dengan 0
    bzero(str, 100);
    // strcpy(str,"# munin node at MyComputer");
  	// bzero(str, strlen(str));
	  // Membaca request dari client
	  read(communication_fd,str,100);	  
	  printf("cap = %d\n", strncmp(str,"cap",strlen("cap")));

	  if (strncmp(str,"cap",strlen("cap")) == 0) { // cap
	  	strcpy(str,"cap multigraph dirtyconfig\n");
	  	printf("%s\n", str);
	  	printf("masuk 1\n");
	  } 
	  else if (strncmp(str,"nodes",strlen("nodes")) == 0) { // nodes
	  	strcpy(str,"MyComputer");
	  	printf("%s\n", str);
	  	printf("masuk 2\n");
	  } 
	  else if (strncmp(str,"list MyComputer",strlen("list MyComputer")) == 0) { // list MyComputer
	  	strcpy(str, "memory");
	  	printf("%s\n", str);
	  	printf("masuk 3\n");
	  } 
	  else if (strncmp(str,"config memory",strlen("config memory")) == 0) { // config memory
	  	write(communication_fd, "graph_args --base 1024 -l 0 --upper-limit ", strlen("graph_args --base 1024 -l 0 --upper-limit ")+1);
      strcpy(str, parse_command(1));
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n"));
      write(communication_fd, "graph_vlabel Bytes\n", strlen("graph_vlabel Bytes")+1);
      write(communication_fd, "graph_title Memory usage\n", strlen("graph_title Memory usage")+1);
      write(communication_fd, "graph_category system\n", strlen("graph_category system")+1);
      write(communication_fd, "graph_info This graph shows this machine memory.\n", strlen("graph_info This graph shows this machine memory.")+1);
      write(communication_fd, "graph_order used free\n", strlen("graph_order used free")+1);
      write(communication_fd, "used.label used\n", strlen("used.label used")+1);
      write(communication_fd, "used.draw STACK\n", strlen("used.draw STACK")+1);
      write(communication_fd, "used.info Used memory.\n", strlen("used.info Used memory.")+1);
      write(communication_fd, "free.label free\n", strlen("free.label free")+1);
      write(communication_fd, "free.draw STACK\n", strlen("free.draw STACK")+1);
      write(communication_fd, "free.info Free memory.\n", strlen("free.info Free memory.")+1);
	  	continue;
	  } 
	  else if (strncmp(str,"fetch memory",strlen("fetch memory")) == 0) { // fetch memory
      write(communication_fd, "used.value ", strlen("used.value "));
	  	strcpy(str, parse_command(2));
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n")+1); // used.value 120933018402
      write(communication_fd, "free.value ", strlen("free.value "));
      strcpy(str, parse_command(3));
      write(communication_fd, str, strlen(str));
      write(communication_fd, "\n", strlen("\n")+1); // free.value 1287498127
      write(communication_fd, ".\n", 2); // .\n
	  	continue;
	  } 
	  else if (strncmp(str,"version",strlen("version")) == 0) { // version
	  	strcpy(str, "TropicalIsland v0.0");
	  } 
	  else if (strncmp(str,"quit", strlen("quit")) == 0) { // quit
	  	strcpy(str, "Connection closed by foreign host\n");
      write(communication_fd, str, strlen(str));
      break;
    }
    else { // otherwise
      strcpy(str, "# Unknown command. Try cap, list, nodes, config, fetch, version or quit\n");
      printf("%s\n", str);
      printf("masuk 4\n");
    }
    // Mengirim response ke client
    write(communication_fd, str, strlen(str));
  }

  shutdown(communication_fd, 2);
  shutdown(listen_fd, 2);

}

// // config memory

// write(communication_fd, "graph_args --base 1024 -l 0 --upper-limit ", strlen("graph_args --base 1024 -l 0 --upper-limit ")+1);
// write(communication_fd, "graph_vlabel Bytes", strlen("graph_vlabel Bytes")+1);
// write(communication_fd, "graph_title Memory usage", strlen("graph_title Memory usage")+1);
// write(communication_fd, "graph_category system", strlen("graph_category system")+1);
// write(communication_fd, "graph_info This graph shows this machine memory.", strlen("graph_info This graph shows this machine memory.")+1);
// write(communication_fd, "graph_order used free", strlen("graph_order used free")+1);
// write(communication_fd, "used.label used", strlen("used.label used")+1);
// write(communication_fd, "used.draw STACK", strlen("used.draw STACK")+1);
// write(communication_fd, "used.info Used memory.", strlen("used.info Used memory.")+1);
// write(communication_fd, "free.label free", strlen("free.label free")+1);
// write(communication_fd, "free.draw STACK", strlen("free.draw STACK")+1);
// write(communication_fd, "free.info Free memory.", strlen("free.info Free memory.")+1);

// // fetch memory

// used.value used.memory
// free.value free.memory