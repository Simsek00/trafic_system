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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TrafficInterface
{
    /// <summary>
    /// TrafficSystemDisplayer.xaml etkileşim mantığı
    /// </summary>
    public partial class TrafficSystemDisplayer : Page
    {
        LightStatus Light1 = new LightStatus();
        LightStatus Light2 = new LightStatus();
        LightStatus Light3 = new LightStatus();
        LightStatus Light4 = new LightStatus();
        public TrafficSystemDisplayer()
        {
            InitializeComponent();
            Light1.LightName.Text = "Light 1";
            Light2.LightName.Text = "Light 2";
            Light3.LightName.Text = "Light 3";
            Light4.LightName.Text = "Light 4";

            Frame1.Navigate(Light1);
            Frame2.Navigate(Light2);
            Frame3.Navigate(Light3);
            Frame4.Navigate(Light4);
            ArdunioSerialPort.VeriGeldi += ArdunioSerialPort_VeriGeldi;

        }

        private void ProfileButton_Click(object sender, RoutedEventArgs e)
        {


            ProfileDisplayerWindow windowprofiledisplayer = new ProfileDisplayerWindow();
                windowprofiledisplayer.Show();
         
           
        }

        private void ArdunioSerialPort_VeriGeldi(string veri)
        {
            // UI güncelleme
            Dispatcher.Invoke(() =>
            {
                int index = veri.IndexOf("SETLIGHTS");
                string result = null;

                if (index != -1)
                {
                    // "SETLIGHTS" kelimesinin bitiminden sonraki kısmı al
                    result = veri.Substring(index + 9).Trim();
                    String[] Lights = result.Split(' ');
                    foreach (string light in Lights)
                    {
                        ParseLightAndColor(light);

                    }
                }
                
            });
        }
        private void ParseLightAndColor(string light)
        {
            String[] lightandcolorparsed = light.Split("_");
            LightStatus? selectedlight = null;
            bool isCarLight = false;
            if (lightandcolorparsed[0] == "C1")
            { 
                selectedlight = Light1; 
                isCarLight = true;
            }
            if (lightandcolorparsed[0] == "C2")
            {
                selectedlight = Light2;
                isCarLight = true;
            }
            if (lightandcolorparsed[0] == "C3")
            {
                selectedlight = Light3;
                isCarLight = true;
            }
            if (lightandcolorparsed[0] == "C4")
            {
                selectedlight = Light4;
                isCarLight = true;
            }
            if (lightandcolorparsed[0] == "P1")
            {
                selectedlight = Light1;
                isCarLight = false;
            }
            if (lightandcolorparsed[0] == "P2")
            {
                selectedlight = Light2;
                isCarLight = false;
            }
            if (lightandcolorparsed[0] == "P3")
            {
                selectedlight = Light3;
                isCarLight = false;
            }
            if (lightandcolorparsed[0] == "P4")
            {
                selectedlight = Light4;
                isCarLight = false;
            }
            if (selectedlight !=null && lightandcolorparsed.Length==2)
            {
                selectedlight.SetTrafficLight(lightandcolorparsed[1],isCarLight);
            }


        }

    }
}
