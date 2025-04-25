using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Windows.Controls;
using System.Threading;
using System.IO;

namespace TrafficInterface
{
    public static class ArdunioSerialPort
    {
        private static SerialPort serialPort;

        public static event Action<string> VeriGeldi;

        public static async Task Baslat(int baudRate = 9600)
        {
            string portName = await ArduinoPortuBulAsync();
            if (string.IsNullOrEmpty(portName))
            {
                Console.WriteLine("Arduino bağlantısı bulunamadı.");
                return;
            }

            Console.WriteLine($"Port bulundu: {portName}");

            serialPort = new SerialPort(portName, baudRate);
            serialPort.DataReceived += SerialPort_DataReceived;

            try
            {
                serialPort.Open();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Seri port açılamadı: {ex.Message}");
            }
        }

        private static void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    string gelenveri = serialPort.ReadExisting();
                    if( !string.IsNullOrWhiteSpace(gelenveri)) 
                    {
                        VeriGeldi?.Invoke(gelenveri);
                    }
                    
                }
                catch (IOException ex)
                {
                    Console.WriteLine($"Veri okuma hatası: {ex.Message}");
                }
            }
        }

        public static async Task<string> ArduinoPortuBulAsync()
        {
            while (true)
            {
                await Task.Delay(300);  // 1 saniye bekler
                foreach (var portName in SerialPort.GetPortNames())
                {
                    try
                    {
                        using (SerialPort port = new SerialPort(portName, 9600))
                        {
                            port.ReadTimeout = 3000;
                            port.WriteTimeout = 3000;
                            port.Open();

                            // Temizlik
                            port.DiscardInBuffer();
                            port.DiscardOutBuffer();

                            // Ping gönder
                            port.WriteLine("ping");

                            // Cevap bekle
                            await Task.Delay(500);  // Arduino’nun cevap vermesi için biraz zaman ver

                            string cevap = port.ReadLine().Trim();

                            if (cevap.Contains("pong"))
                            {
                                port.Close();  // Portu kapatmadan önce portName'ı döndür
                                return portName;  // Doğru port bulundu
                            }

                            port.Close();
                        }
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"Port hatası: {ex.Message}");
                    }
                }

                
            }
        }
    }
}