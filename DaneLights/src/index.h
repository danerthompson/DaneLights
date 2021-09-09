//May need to use PROGMEM

const char index_html[] = R"rawliteral(
<!DOCTYPE HTML><html>

<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <script src="https://cdn.jsdelivr.net/npm/@jaames/iro@5"></script>
  <style>
    h1 {text-align: center;}
    p {text-align: center;}
    div {margin: auto;}
  </style>
</head>

<body style="background-color:rgb(45, 45, 48);">

<h1 style="color:white"> Dane's Lights </h1>
<p style="color:white">Powered by ESP32</p>

<div class="ColorPicker" id="defaultPicker" style="width=100%;height=300px;"></div>

<p id="currentColor", style="color:white"> </p>


<script>
    var defaultPicker;
    var ledColor;
    var response;
    var prevColor = 0;

    readPreferences(); //Load color picker with preferences

    setInterval( function() {
        myFunction();
    }, 100);


    function myFunction() {

        ledColor = defaultPicker.color.hexString;
        if (ledColor != prevColor) {
            document.getElementById("currentColor").innerHTML = ledColor;
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/lights/update?hex=" + ledColor.substring(1), true); //Have to remove first char from x, server detects '#' as delimiter
            xhttp.send();
            prevColor = ledColor;
        }

    }

    function readPreferences() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (xhttp.readyState == 4 && xhttp.status == 200) {
                response = xhttp.responseText;
                defaultPicker = new iro.ColorPicker("#defaultPicker", {
                    width: 300,
                    color: response,
                    borderWidth: 1,
                    borderColor: "#fff",
                });
            }
        }
        xhttp.open("GET", "/lights/initialize", true);
        xhttp.responseType = 'text';
        xhttp.send();

    }
</script>

</body>
</html>
)rawliteral";