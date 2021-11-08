#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <mysql.h>
#include <pthread.h>

typedef struct {
	char nombre[20];
	int socket;
}Conectado;

typedef struct {
	Conectado conectados [100];
	int num;
}ListaConectados;

ListaConectados lista;

int Conectarse (ListaConectados *lista, char nombre[20], int socket){
	if (lista->num == 200)
		return -1;
	else {
		strcpy (lista->conectados[lista->num].nombre, nombre);
		lista->conectados[lista->num].socket = socket;
		lista->num++;
		return 0;
	}
}

int DevuelveSocket (ListaConectados *lista, char nombre[20]){
	
	int i=0;
	int encontrado = 0;
	while ((i< lista->num) && !encontrado)
	{
		if (strcmp(lista->conectados[i].nombre, nombre) == 0)
			encontrado =1;
		if (!encontrado)
			i=i+1;
	}
	if (encontrado)
		return lista->conectados[i].socket;
	else
		return -1;
}


int DevuelvePosicion (ListaConectados *lista, char nombre[20]){
	
	int i=0;
	int encontrado = 0;
	while ((i< lista->num) && !encontrado)
	{
		if (strcmp(lista->conectados[i].nombre, nombre) == 0)
			encontrado =1;
		if (!encontrado)
			i=i+1;
	}
	if (encontrado)
		return i;
	else
		return -1;
}


int Eliminar (ListaConectados *lista, char nombre[20]){
 // Elimina los usuarios que se acaban de registrar al cerrar la sesión. 
// Devuelve -1 si no ha funcionado y 0 si todo ha ido bien
	int pos = DevuelvePosicion (lista, nombre);
	if (pos == -1)
		return -1;
	else
	{
		int i;
		for (i=pos; i< lista->num-1; i++)
		{
			strcpy (lista->conectados[i].nombre, lista->conectados[i+1].nombre);
			lista->conectados[i].socket = lista->conectados[i+1].socket;          
		}
		lista->num--;
		return 0;
	}
}


void DevuelveConectados (ListaConectados *lista, char conectados[500]){
	
	sprintf (conectados, "Numero de personas conectadas: %d \n", lista->num);
	int i;
	for (i=0; i< lista->num; i++)
	{
		sprintf (conectados, "%s \n %s", conectados, lista->conectados[i].nombre);
	}
}

void *FuncionPrincipal (void *socket)
{
	
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	int error;
	char peticion[600];
	char respuesta[600];
	int ret;
	int terminar =0;
	
	
	while (terminar ==0)
	{
		
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		peticion[ret]='\0';
		
		printf ("Se ha conectado: %s\n",peticion);
		
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		char nombre[20];
		p = strtok( NULL, "/");
		
		strcpy (nombre, p);
		printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		
		switch (codigo)
		{
		case 0:
			terminar = 1;
			Eliminar(&lista,nombre);
			close(sock_conn);
			break;
		case 1:
		// Con el caso 1 se registra un nuevo usuario
			p = strtok( NULL, "/");
			char contrasena[20];
			strcpy (contrasena, p);
			error = Registrarse(nombre,contrasena);
			write (sock_conn,nombre, strlen(nombre));
			
			if (error != 0)
				printf ("Ha ocurrido un error en el caso 1");
			
			break;
			
		case 2:
			//En este caso se Inicia Sesion un usuario ya registrado
			p = strtok( NULL, "/");
			
			strcpy (contrasena, p);
			error = LogIn(nombre,contrasena);
			write (sock_conn,nombre,strlen(nombre));
			int numsocket = DevuelveSocket(&lista,nombre);
			int errorCON = Conectarse (&lista,nombre,numsocket);
			if (error != 0)
				printf ("Ha ocurrido un error en el caso 2");
			if (errorCON != 0)
				printf ("Ha ocurrido un error en el caso 1, no se ha podido añadir a la lista de conectados");
			
			break;
		case 3:
			// Nuestra primera consulta: El nombre de la persona que más partidas ha ganado
			error = GanadorAbsoluto(nombre,respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
			
			if (error != 0)
				printf ("Ha ocurrido un error en el caso 3");
			
			break;
			
		case 4:
			//Nos pide el numero de las partidas ganadas en Febrero
			error = PartidasFebrero(nombre,respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
			
			if (error != 0)
				printf ("Ha ocurrido un error en el caso 4");
			
			break;
		case 5:
			// Partidas en las que Juan ha obtenido mas de 30 puntos
			error = JuanTrentaPuntos(nombre,respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
			
			if (error != 0)
				printf ("Ha ocurrido un error en el caso 5");
			break;
		case 6:
			DevuelveConectados(&lista,respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
			break;
			
		default:
			
			break;
		}
		
	}
	
	close(sock_conn); 
	
}


int GanadorAbsoluto(char nombre[20], char resultado[300])
{
	MYSQL *conn;
	int err;
	MYSQL_RES *resultado_c;
	MYSQL_ROW row;
	char consulta [300];
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego2",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	sprintf (consulta, "SELECT ganador, COUNT( ganador ) AS total FROM  Partidas GROUP BY ganador ORDER BY total DESC;",nombre);
	
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		sprintf (resultado,"Error al consultar la base de datos %u %s",
				 mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado_c = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado_c);
	
	if (row == NULL) {
		sprintf (resultado,"No se han obtenido datos en la consulta\n");
		
	}
	else
		strcpy(resultado,row[0]);
	
	return 0;
}

int PartidasFebrero(char nombre[20], char resultado[300])
{
	MYSQL *conn;
	int err;
	MYSQL_RES *resultado_c;
	MYSQL_ROW row;
	char consulta [300];
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}

	conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego2",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}

	sprintf (consulta, "SELECT COUNT(*) FROM Partidas WHERE fecha = 02;",nombre);
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		printf ("Error al consultar la base de datos %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado_c = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado_c);
	if (row == NULL){
		sprintf (resultado,"No se han obtenido datos en la consulta\n");
	}
	else
		sprintf(resultado,row[0]); 
	return 0;
}

int JuanTrentaPuntos(char nombre[20], char resultado[300])
{
	MYSQL *conn;
	int err;
	MYSQL_RES *resultado_c;
	MYSQL_ROW row;
	char consulta [300];
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego2",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	sprintf (consulta, "SELECT COUNT(*) FROM Partidas WHERE ganador ='Juan' AND puntosganador >30;",nombre);
	
	err=mysql_query (conn, consulta); 
	if (err!=0) {
		sprintf (resultado,"Error al consultar la base de datos %u %s",
				 mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado_c = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado_c);
	
	if (row == NULL){
		sprintf (resultado,"No se han obtenido datos en la consulta\n");
	}
	else
		strcpy(resultado,row[0]); 
	return 0;
	
}

int Registrarse (char usuario[20], char contrasena[20]) {	
	MYSQL *conn;
	MYSQL_RES *resultado_c;
	MYSQL_ROW row;
	int err;
	char consulta [80];
	
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego2",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	err=mysql_query (conn, "SELECT * FROM Jugador");
	if (err!=0) {
		printf ("Error al consultar la base de datos %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado_c = mysql_store_result (conn);
	int i = 0;
	row = mysql_fetch_row (resultado_c);
	while(row != NULL) {
		i++;
		row = mysql_fetch_row (resultado_c);
	}
	strcpy(consulta, "SELECT * FROM Jugador WHERE Jugador.nombre ='");
	strcat(consulta, usuario);
	strcat(consulta, "'");
	err=mysql_query (conn, consulta);
	if (err!=0) {
		printf ("Error al consultar la base de datos %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado_c = mysql_store_result (conn);
	row = mysql_fetch_row (resultado_c);
	if(row != NULL) {
		printf("Error. Ya existe este nombre.");
		exit (1);
	}
	sprintf(consulta, "INSERT INTO Jugador (id,nombre,password) VALUES ('%d','%s','%s');", i + 1, usuario, contrasena);
	
	printf("consulta = %s\n", consulta);
	err = mysql_query(conn, consulta);
	if (err!=0) {
		printf ("Error al introducir datos la base %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	return 0;	
}

int LogIn(char usuario[20], char contrasena[20]) {
	MYSQL *conn;
	MYSQL_RES *resultado_c;
	MYSQL_ROW row;
	int err;
	char consulta [150];
	 
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "Juego2",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	strcpy(consulta, "SELECT * FROM Jugador WHERE Jugador.nombre='");
	strcat(consulta, usuario);
	strcat(consulta, "' AND Jugador.password='");
	strcat(consulta, contrasena);
	strcat(consulta, "'");
	err=mysql_query (conn, consulta);
	if (err!=0) {
		printf ("Error al consultar datos de la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado_c = mysql_store_result (conn);
	row = mysql_fetch_row (resultado_c);
	if(row == NULL) {
		printf("Error. Los datos no coinciden.");
		exit (1);
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	lista.num=0;
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");

	
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	if (listen(sock_listen, 2) < 0)
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	pthread_t thread;
	i=0;
	
	for(;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		
		pthread_create (&thread, NULL, FuncionPrincipal,&sockets[i]);
		i=i+1;
		
	}
	
}

