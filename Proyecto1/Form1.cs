using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;


namespace Cliente
{
    public partial class Form1 : Form
    {
        public static Socket server;
        public static int A;
        public static string N;
        bool registrado = false;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
        private void button1_Click(object sender, EventArgs e) //Boton para conectar con el servidor
        {

            //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
            //al que deseamos conectarnos
            IPAddress direc = IPAddress.Parse("192.168.56.102");
            IPEndPoint ipep = new IPEndPoint(direc, 9050);


            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.BackColor = Color.Green;
                MessageBox.Show("Conectado");

            }
            catch (SocketException ex)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No se ha conectado con el servidor");
                return;
            }

        }

        private void button2_Click(object sender, EventArgs e) //Boton para saber que consulta queremos resolver
        {

            
                if (ganado.Checked) //Primera consulta
                {
                    string mensaje = "3/" + Form1.N;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    Form1.server.Send(msg);

                    //Recibimos la respuesta del servidor
                    byte[] msg2 = new byte[80];
                    Form1.server.Receive(msg2);
                    mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show("El que ha ganado más partidas es " + mensaje);

                }
                else if (febrero.Checked) //Segunda consulta
                {
                    string mensaje = "4/" + Form1.N;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    Form1.server.Send(msg);

                    //Recibimos la respuesta del servidor
                    byte[] msg2 = new byte[80];
                    Form1.server.Receive(msg2);
                    mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show(Form1.N + " Las partidas ganadas en febrero:" + mensaje );
                }
                else if (puntos.Checked)// Tercera consulta
                {
                    string mensaje = "5/" + Form1.N;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    Form1.server.Send(msg);

                    //Recibimos la respuesta del servidor
                    byte[] msg2 = new byte[80];
                    Form1.server.Receive(msg2);
                    mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show(Form1.N + " El número de partidas en las que Juan ha obtenido más de 30 puntos es " + mensaje );
                }
            
            
        }






        private void button3_Click(object sender, EventArgs e) //boton para iniciar sesion
        {


            if (A != 1)
            {
                if (registrado == false)
                {



                    string mensaje = "2/" + usuario.Text + "/" + password.Text;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    server.Send(msg);

                    //Recibimos la respuesta del servidor
                    byte[] msg2 = new byte[80];
                    server.Receive(msg2);
                    mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                    MessageBox.Show(mensaje + " ha iniciado sesión correctamente");
                    Form1.A = 1;
                    Form1.N = usuario.Text;



                    this.BackColor = Color.Pink;
                    MessageBox.Show("Bienvenido :)");

                }
                else
                    MessageBox.Show("Ya hay una sesión iniciada.");
            }
        }

        

        private void button4_Click(object sender, EventArgs e) //Boton para registarse
        {
            

            

            string mensaje = "1/" + usuario.Text + "/" + password.Text;
            // Enviamos al servidor el nombre tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //Recibimos la respuesta del servidor
            byte[] msg2 = new byte[80];
            server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
            this.BackColor = Color.Pink;
            MessageBox.Show(mensaje+" se ha registrado correctamente!");
            registrado = true;
        
        }

        private void button5_Click(object sender, EventArgs e) //Lista de personas conectadas
        {
            string mensaje = "6/" + Form1.N;
            // Enviamos al servidor el nombre tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            Form1.server.Send(msg);

            //Recibimos la respuesta del servidor
            byte[] msg2 = new byte[80];
            Form1.server.Receive(msg2);
            mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
            MessageBox.Show(mensaje);
        }
        
}
        


           


                

            
        
    }




