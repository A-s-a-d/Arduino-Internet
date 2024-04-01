const button_API_ref = document.getElementById("button_API_id");
const cityinfos_ref = document.getElementById("City")
const API_temperature_ref = document.getElementById("API_temperature_id");
const API_cloud_ref = document.getElementById("API_cloud_id");
const API_icon_ref = document.getElementById("API_icon_id");
const temperature_ref = document.getElementById("temperature_id");
const delay = 1000;
let datas = [];

button_API_ref.addEventListener("click", (event) => {
  const value = cityinfos_ref.value;
  console.log(value);

  const xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/cityinfos?cityName=" + value, true);
  xhttp.send();

  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      const jsonCity = JSON.parse(this.responseText);
      console.log(this.responseText);
      API_temperature_ref.textContent = jsonCity.main.temp;

      const iconURL = "https://openweathermap.org/img/wn/" + jsonCity.weather[0].icon + "@2x.png";
      API_icon_ref.src = iconURL;
      API_cloud_ref.textContent = jsonCity.weather[0].description;
    }
  }
});

setInterval(() => {
  const xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/temperature", true);
  xhttp.send();

  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      const value = this.responseText;
      console.log(this.responseText);
      temperature_ref.textContent = value;
      gaugeTemperature.refresh(value);
    }
  }
}, delay);

let myChart; // Déclaration d'une variable globale pour stocker le graphique

setInterval(() => {
    let dataGraphique = [];
    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/graphique", true);
    xhttp.send();

    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            dataGraphique = JSON.parse(this.responseText); 
            console.log(this.responseText);

            const ctx = document.getElementById('Graphique').getContext('2d');

            // Vérifier si un graphique existe déjà sur ce canvas
            if (myChart) {
                // Si un graphique existe, le détruire
                myChart.destroy();
            }

            // Données du graphique
            let data = {
                labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20'],
                datasets: [{
                    label: 'Température (en °C)',
                    data: dataGraphique,
                    borderWidth: 1
                }]
            };

            // Options du graphique
            let options = {
                scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero: true
                        }
                    }]
                }
            };

            // Création du graphique et stockage dans la variable globale
            myChart = new Chart(ctx, {
                type: 'line',
                data: data,
                options: options
            });
        }
    }
}, 20001);


const gaugeTemperature = new JustGage({
  id: "gauge_temperature",
  value: 0,
  min: -10,
  max: 50,
  decimals: 2,
  title: "",
  label: "°C",
  relativeGaugeSize: true,
  gaugeWidthScale: 0.6,
});