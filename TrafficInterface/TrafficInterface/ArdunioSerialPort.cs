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

        private static String bekleyenveri;

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
                    bekleyenveri += gelenveri;

                    if (bekleyenveri.Contains("\n"))
                    {
                        bekleyenveri.Remove(0);
                        if (!string.IsNullOrWhiteSpace(bekleyenveri))
                        {
                            
                            VeriGeldi?.Invoke(bekleyenveri);
                        }
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
                            // Port ayarları
                            port.ReadTimeout = 1000;
                            port.WriteTimeout = 1000;

                            if (!port.IsOpen)
                            {
                                port.Open(); // Eğer açık değilse aç
                            }

                            // Temizlik
                            port.DiscardInBuffer();
                            port.DiscardOutBuffer();

                            // Ping gönder
                            port.WriteLine("ping");

                            // Cevap bekle, zamanlayıcıyı artırarak daha fazla bekle
                            await Task.Delay(1000);  // Arduino’nun cevap vermesi için biraz daha fazla zaman ver

                            // Cevap oku
                            string cevap = port.ReadExisting();

                            // Eğer "pong" cevabını alırsak doğru port bulduk
                            if (cevap.Contains("pong"))
                            {
                                port.Close();  // Portu kapat
                                return portName;  // Doğru port bulundu
                            }

                            // Eğer "pong" cevabı gelmediyse, portu kapat
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