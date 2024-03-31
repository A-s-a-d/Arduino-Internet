const button_on_ref = document.getElementById("button_on_id");
const button_off_ref = document.getElementById("button_off_id");
const button_API_ref = document.getElementById("button_API_id");
const cityinfos_ref = document.getElementById("City")
const API_temperature_ref = document.getElementById("API_temperature_id");
const API_cloud_ref = document.getElementById("API_cloud_id");
const API_icon_ref = document.getElementById("API_icon_id");
const temperature_ref = document.getElementById("temperature_id");
const delay = 1000;
let datas = [];

button_on_ref.addEventListener("click", (event) => {
  console.log("On");
  button_on_ref.classList.add("w3-disabled");
  button_off_ref.classList.remove("w3-disabled");
  const xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/button?state=1", true);
  xhttp.send();
});

button_off_ref.addEventListener("click", (event) => {
  console.log("Off");
  button_on_ref.classList.remove("w3-disabled");
  button_off_ref.classList.add("w3-disabled");
  const xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/button?state=0", true);
  xhttp.send();
});

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

setInterval(() => {
  let dataGraphique = [12, 19, 3, 5, 2, 3];
  const xhttp = new XMLHttpRequest();
  xhttp.open("GET", "/graphique", true);
  xhttp.send();

  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      dataGraphique = this.responseText; // Assurez-vous de convertir la réponse en JSON si elle est au format JSON
      console.log(this.responseText);

      const ctx = document.getElementById('graphique');

      if (ctx && ctx.chart) {
        // Si un graphique existe, le détruire
        ctx.chart.destroy();
      }
      
      let newctx = new Chart(ctx, {
        type: 'line',
        data: {
          labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20'],
          datasets: [{
            label: 'Température (en °C)',
            data: dataGraphique,
            borderWidth: 1
          }]
        },
        options: {
          scales: {
            y: {
              beginAtZero: true
            }
          }
        }
      });

      ctx.chart = newctx;
    }
  }
}, 5000);


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