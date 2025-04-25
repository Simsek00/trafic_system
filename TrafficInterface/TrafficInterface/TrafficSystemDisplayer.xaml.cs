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

            Frame1.Navigate(Light1);
            Frame2.Navigate(Light2);
            Frame3.Navigate(Light3);
            Frame4.Navigate(Light4);
        }

        private void ProfileButton_Click(object sender, RoutedEventArgs e)
        {


            ProfileDisplayerWindow windowprofiledisplayer = new ProfileDisplayerWindow();
                windowprofiledisplayer.Show();
         
           
        }
    }
}
