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
    /// LightStatus.xaml etkileşim mantığı
    /// </summary>
    public partial class LightStatus : Page
    {
        public LightStatus()
        {
            InitializeComponent();
            
        }


        public void SetTrafficLight(string LightColor, bool isCarLight=true)
        {
            if (LightColor == "R")
            {
                ChangeBorderBackground("resources/trafficlightart-02.png", "resources/trafficlightart-11.png", isCarLight);
            }
            if (LightColor == "Y")
            {
                ChangeBorderBackground("resources/trafficlightart-03.png", "resources/trafficlightart-11.png", isCarLight);
            }
            if (LightColor == "G")
            {
                ChangeBorderBackground("resources/trafficlightart-04.png", "resources/trafficlightart-12.png", isCarLight);
            }
        }


        private void ChangeBorderBackground(string imagePath, string imagePath2, bool isCarLight=true)
        {

            // BitmapImage oluştur
            BitmapImage bitmap = new BitmapImage();
            bitmap.BeginInit();
            if(isCarLight)
                bitmap.UriSource = new Uri(imagePath, UriKind.Relative);
            else
            {
                bitmap.UriSource = new Uri(imagePath2, UriKind.Relative);
            }
            bitmap.EndInit();

            // ImageBrush oluştur
            ImageBrush imageBrush = new ImageBrush(bitmap)
            {
                Stretch = Stretch.UniformToFill // İsteğe göre değiştirebilirsin
            };

            if (isCarLight) 
            {
                CarLight.Background = imageBrush;
            }
            else
            {
                P_Light.Background = imageBrush;
            }

        }



    }
}
