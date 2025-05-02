using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Emit;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TrafficInterface
{
    /// <summary>
    /// MainMenu.xaml etkileşim mantığı
    /// </summary>
    public partial class MainMenu : Page
    {
        public MainMenu()
        {
            InitializeComponent();
            ArdunioSerialPort.VeriGeldi += ArdunioSerialPort_VeriGeldi;
            ArdunioSerialPort.Baslat(9600);
            FrameMain.Visibility = Visibility.Hidden;
        }

        private void Button_Click (object sender, RoutedEventArgs e)
        {
            TrafficSystemDisplayer pageTrafficSystem = new TrafficSystemDisplayer(FrameMain);
            FrameMain.Navigate(pageTrafficSystem);
            pageTrafficSystem.Focus();
            FrameMain.Visibility = Visibility.Visible;

        }
        private void ArdunioSerialPort_VeriGeldi(string veri)
        {
            // UI güncelleme
            Dispatcher.Invoke(() =>
            {
                Console.WriteLine("Islem:");
                Console.Write(veri);
            });
        }
    }
}
