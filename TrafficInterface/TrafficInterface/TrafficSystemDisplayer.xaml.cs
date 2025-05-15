using System;
using System.Collections.Generic;
using System.Configuration;
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
        int Yayabutonsayisi = 0;
        LightStatus Light1 = new LightStatus();
        LightStatus Light2 = new LightStatus();
        LightStatus Light3 = new LightStatus();
        LightStatus Light4 = new LightStatus();
        Frame backframe;
        public TrafficSystemDisplayer(Frame lastframe)
        {
            InitializeComponent();
            this.PreviewKeyDown += Page_PreviewKeyDown;
            this.Focusable = true;
            this.Loaded += (s, e) => Keyboard.Focus(this);
            backframe = lastframe as Frame;
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
                // YAYA BUTON TIKLANMA SAYISI
                index = veri.IndexOf("3 Nolu yaya talebi");
                if (index != -1)
                {
                    Yayabutonsayisi++;
                    Light1.YayaButonText.Text = Yayabutonsayisi.ToString();
                    Light2.YayaButonText.Text = Yayabutonsayisi.ToString();
                    Light3.YayaButonText.Text = Yayabutonsayisi.ToString();
                    Light4.YayaButonText.Text = Yayabutonsayisi.ToString();
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
        private void Page_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                backframe.NavigationService.GoBack();
                backframe.Visibility = Visibility.Hidden;
            }
        }

        private void ButtonBack_Click(object sender, RoutedEventArgs e)
        {
            backframe.NavigationService.GoBack();
            backframe.Visibility = Visibility.Hidden;
        }
    }
}
