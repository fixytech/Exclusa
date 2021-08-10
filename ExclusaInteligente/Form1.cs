using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;
using System.Threading;

namespace ExclusaInteligente
{
    public partial class Principal : Form
    {
        String MP = "0";
        String MPI = "0";
        String MPE = "1";
        String EP = "0";
        String UL = "0";

        public Principal()
        {
            InitializeComponent();
            

        }

       
       
        private void btnConectar_Click(object sender, EventArgs e)
        {

            // Asignamos las propiedades
            serialPort1.BaudRate = 9600;
            serialPort1.PortName = tbCOM.Text;
           

            // Controlamos que el puerto indicado esté operativo
            try
            {
                // Abrimos el puerto serie
                serialPort1.Open();
                toolStripStatusLabel1.Text = "Dispositivo conectado!";
                btnConectar.Enabled = false;
                btnConectar.Text = "Conectado!";
                btnDesconectar.Enabled = true;

                Thread Hilo = new Thread(escucharSerial);
                Hilo.Start();
            }
            catch
            {
                MessageBox.Show("Puerto incorrecto.");
                toolStripStatusLabel1.Text = "Puerto incorrecto.";
            }
        }

       private void escucharSerial()
        {
         
            String cadena;
            try
            {
          
                while (serialPort1.IsOpen)
                {
                    txtLogs.Invoke(new MethodInvoker(
                        delegate{
                            try
                            {
                                
                                cadena = serialPort1.ReadLine();

                                textBox1.Text = textBox1.Text.Insert(this.textBox1.SelectionStart, DateTime.Now.ToString("hh:mm:ss") + " | " + cadena + "\r\n");
                             
                                try
                                {
                                    string[] estadoMovimiento = cadena.Split('M');
                                    estadoMovimiento = estadoMovimiento[1].Split(',');
                                    string EPP = estadoMovimiento[0];
                                    if(EPP == "1")
                                    {
                                        EP = "1";
                                        panel1.BackColor = Color.Red;
                                        lblMov.Text = "PIR: Alarma!";
                                    }
                                    if(EPP == "0")
                                    {
                                        EP = "0";
                                        panel1.BackColor = Color.LightGray;
                                        lblMov.Text = "PIR: OK";
                                    }
                                }
                                catch
                                {

                                }

                                try
                                {
                                    string[] estadoPuerta = cadena.Split('P');
                                    estadoPuerta = estadoPuerta[1].Split(',');
                                    string EP = estadoPuerta[0];
                                    
                                    if (EP == "0")
                                    {
                                        MP = "1";
                                        panel3.BackColor = Color.Red;
                                        lblPta.Text = "Puerta: Abierta! ->";
                                    }
                                    if (EP == "1")
                                    {
                                        MP = "0";
                                        panel3.BackColor = Color.Green;
                                        lblPta.Text = "Puerta: Cerrada. ->";
                                    }
                                }
                                catch
                                {

                                }

                                try
                                {
                                    string[] estadoPInterior = cadena.Split('I');
                                    estadoPInterior = estadoPInterior[1].Split(',');
                                    string EPI = estadoPInterior[0];

                                    if (EPI == "0")
                                    {
                                        MPI = "1";
                                        panel4.BackColor = Color.Red;
                                        lblPi.Text = "Persiana: Abierta!";
                                    }
                                    if (EPI == "1")
                                    {
                                        MPI = "0";
                                        panel4.BackColor = Color.Green;
                                        lblPi.Text = "Persiana: Cerrada.";
                                    }
                                }
                                catch
                                {

                                }

                                try
                                {
                                    string[] estadoPExterior = cadena.Split('E');
                                    estadoPExterior = estadoPExterior[1].Split(',');
                                    string EPE = estadoPExterior[0];

                                    if (EPE == "0")
                                    {
                                        MPE = "1";
                                        panel5.BackColor = Color.Red;
                                        lblPe.Text = "Persiana: Abierta!";
                                    }
                                    if (EPE == "1")
                                    {
                                        MPE = "0";
                                        panel5.BackColor = Color.Green;
                                        lblPe.Text = "Persiana: Cerrada.";
                                    }
                                }
                                catch
                                {

                                }
                                try
                                {
                                    string[] estadoUltimoingreso = cadena.Split('U');
                                    estadoUltimoingreso = estadoUltimoingreso[1].Split(',');
                                    UL = estadoUltimoingreso[0];

                                    if (UL == "2")
                                    {
                                       
                                        
                                        label7.Text = "PUERTA";
                                    }
                                    if (UL == "1")
                                    {
                                        label7.Text = "PERSIANA";
                                    }
                                }
                                catch
                                {

                                }

                                try
                                {
                                    string[] estadoProceso = cadena.Split('$');
                                    estadoProceso = estadoProceso[1].Split('&');
                                    string PRO = estadoProceso[0];

                                        toolStripStatusLabel1.Text = PRO;
                              
                                }
                                catch
                                {

                                }


                                txtLogs.Text = "Movimiento: " + EP + String.Format(Environment.NewLine) + 
                                "Puerta: " + MP + String.Format(Environment.NewLine) + 
                                "Persiana Int: " + MPI + String.Format(Environment.NewLine) +
                                "Persiana Ext: " + MPE + String.Format(Environment.NewLine);

                            }
                            catch(InvalidOperationException ex)
                            {
                                MessageBox.Show(ex.Message);
                            }
                           
                    }
                            ));

                    Thread.Sleep(1); //ms
                }
            }
            catch
            {

            }
            
        }

        private void btnDesconectar_Click(object sender, EventArgs e)
        {
            try
            {
                // cerramos el puerto serie
              
                serialPort1.Close();
                btnConectar.Enabled = true;
                btnDesconectar.Enabled = false;
                btnConectar.Text = "Conectar";
                toolStripStatusLabel1.Text = "Dispositivo desconectado.";

            }
            catch
            {
                MessageBox.Show("No se pudo desconectar!.");
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            toolStripStatusLabel1.Text = "Conecte el dispositivo";
        }
    }
}
