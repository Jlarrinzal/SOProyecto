DROP DATABASE IF EXISTS Juego2;
CREATE DATABASE Juego2;
USE Juego2;
CREATE TABLE Jugador(
id INTEGER,
nombre VARCHAR(20),
password VARCHAR(20),
PRIMARY KEY (id)
)ENGINE=InnoDB;

CREATE TABLE Partidas(
id INTEGER NOT NULL,
fecha INTEGER,   
hora VARCHAR(20),
duracion INTEGER(20),
ganador VARCHAR(20),
puntosganador INTEGER NOT NULL,
puntosperdedor INTEGER NOT NULL,
PRIMARY KEY (id)
)ENGINE=InnoDB;

CREATE TABLE Datos(
idJu INTEGER NOT NULL,
idPa INTEGER NOT NULL,
FOREIGN KEY (idJu) REFERENCES   Jugador(id),
FOREIGN KEY (idPa) REFERENCES   Partidas(id)
)ENGINE=InnoDB;

INSERT INTO Jugador(id,nombre,password) VALUES(1,'Juan','perrito1');
INSERT INTO Jugador(id,nombre,password) VALUES(2,'Pablo','compañero1');
INSERT INTO Jugador(id,nombre,password) VALUES(3,'Alberto','anuel');
INSERT INTO Partidas(id,fecha,hora,duracion,ganador,puntosganador,puntosperdedor) VALUES(1,02,'14:00',47,'Pablo',50,25);
INSERT INTO Partidas(id,fecha,hora,duracion,ganador,puntosganador,puntosperdedor) VALUES(2,02,'18:00',24,'Juan',40,28);
INSERT INTO Partidas(id,fecha,hora,duracion,ganador,puntosganador,puntosperdedor) VALUES(3,10,'19:00',30,'Juan',20,18);

