using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace TrafficInterface
{
    /// <summary>
    /// ProfileDisplayerWindow.xaml etkileşim mantığı
    /// </summary>
    public partial class ProfileDisplayerWindow : Window
    {
        Profile profile1 = new Profile();
        public ProfileDisplayerWindow()
        {
            InitializeComponent();
            
            ProfileFrame.Navigate(profile1);
            profile1.IsEnabled = false;
            myComboBox.SelectedIndex = 0;

        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox comboBox = sender as ComboBox;

            if (comboBox.SelectedItem is ComboBoxItem selectedItem)
            {
                string secilenDeger = selectedItem.Content.ToString();
                if (secilenDeger =="Özel Profil Tanımla")
                {
                    profile1.IsEnabled=true;
                }
                else
                    profile1.IsEnabled = false;
                if (secilenDeger == "Yoğun Araç Trafiği") 
                {
                    SetProfile("15", "2", "10");
                }
                if (secilenDeger == "Orta Araç Trafiği") {
                    SetProfile("10", "2", "10");
                }
                if (secilenDeger == "Az Araç Trafiği") {
                    SetProfile("5", "2", "10");
                }
                if (secilenDeger == "Yoğun Yaya Trafiği") {
                    SetProfile("20", "2", "10");
                }
                if (secilenDeger == "Az Yaya Trafiği") {
                    SetProfile("15", "2", "10");
                }

            }
        }
        private void SetProfile(String CRLight, String CYLight, String CGLight)
        {
            profile1.CarRLight.Text = CRLight;
            profile1.CarYLight.Text = CYLight;
            profile1.CarGLight.Text = CGLight;
            ArdunioSerialPort.serialPort.WriteLine("ELLE:"+" "+CRLight+" "+CYLight+" "+CGLight);
           
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
