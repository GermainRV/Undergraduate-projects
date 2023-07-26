const char* host = "esp32";
// const char* ssid = "Germain's Redmi Note 12";
// const char* password = "011235WiFi";
const char* ssid = "POCOM3";
const char* password = "12345678";
// const char* ssid = "Germain's Laptop";
// const char* password = "12345678";

/*
 * Main page
 */

const char* MainPageCode = R"=====(
<html>
<head>
<!-- <meta http-equiv="refresh" content="5"> -->
<style>
  body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 20px; }
  form { margin-top: 10px; }
  input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; cursor: pointer; }
  .outer-container {
    display: flex;
    justify-content: center;
    align-items: center;
/*     width: 100vw; */
/*     height: 100vh; Set the height of the container to the full viewport height */
  }
  .control-container { background-color: #ffffff; border-radius: 20px; padding: 10px; margin-bottom: 10px; text-align: center; }
  .control-title { margin-top: 5px; margin-bottom: 5px; }
  .control-button { background-color: #4CAF50; color: white; padding: 8px 12px; border: none; cursor: pointer; margin-right: 10px;  margin-bottom: -10px;}
  .control-button.sensor-off { background-color: #f44336;margin-right: -5px; }
  .update-section {
    margin-top: 20px;
    margin-bottom: 20px;
/*     font-size: 20px; */
    border: 1px solid #ccc;
    padding: 10px;
    background-color: #f0f0f0;
    border-radius: 20px;
/*     white-space: pre-wrap; */
    word-wrap: break-word;
/*     max-width: 500px; */
    width: 75%;
  }
  #prg { font-size: 16px; }
  #timer {
    position: absolute;
    top: 0;
    right: 0;
    background-color: #000;
    color: #fff;
    padding: 5px;
    font-size: 14px;
  }
</style>
</head>
<body onload="startTime()">
<div id="timer"></div>
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
  <input type='file' name='update'>
  <input type='submit' value='Update'>
</form>
<div id='prg'>Progress: 0%</div>
<div class="outer-container">
  <div class='update-section' id='batteryInfo' style='text-align: center;'>
    <h2>Sensor Readings</h2>
    <table style='margin: auto; border-collapse: collapse; width: 90%;'>
      <tr>
        <th style='border: 1px solid black; padding: 8px;'>Sensor</th>
        <th style='border: 1px solid black; padding: 8px;'>Value</th>
      </tr> 
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>MAX17043</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%MAX17043_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>DHT11</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%DHT11_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>TEMT6000</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%TEMT6000_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>PIR</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%PIR_VALUE%</td>
      </tr>
    </table>
  </div>
</div>
  
<div class="outer-container">
  <div class='control-container' id='sensor-container-1' style='width: 300px; text-align: center'>
    <h2 class='control-title'>Sensor Control</h2>
    <table style='margin: auto;'>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>DHT11</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='DHTstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='DHTstate' value='off'>Turn off</button>
    </form></th>
      </tr>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>TEMT6000</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='TEMTstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='TEMTstate' value='off'>Turn off</button>
    </form></th>
      </tr>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>PIR</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='PIRstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='PIRstate' value='off'>Turn off</button>
    </form></th>
      </tr>
    </table>
  </div>
</div>
<div class="outer-container">
  <div class='control-container' id='sensor-container-1' style='width: 190px;'>
    <h2 class='control-title'>Load Control</h2>
    <form method='get'>
      <button class='control-button' type='submit' name='pinLoad' value='on'>Turn on</button>
      <button class='control-button sensor-off' type='submit' name='pinLoad' value='off'>Turn off</button>
    </form>
  </div>
  </div>
<script>
$('form').submit(function(e){
e.preventDefault();
var form = $('#upload_form')[0];
var data = new FormData(form);
$.ajax({
url: '/update',
type: 'POST',
data: data,
contentType: false,
processData:false,
xhr: function() {
var xhr = new window.XMLHttpRequest();
xhr.upload.addEventListener('progress', function(evt) {
if (evt.lengthComputable) {
var per = evt.loaded / evt.total;
$('#prg').html('progress: ' + Math.round(per*100) + '%');

}, false);
return xhr;
},
success:function(d, s) {
console.log('success!');
},
error: function (a, b, c) {
}
});
}
</script>
<script>
  function startTime() {
    const today = new Date();
    let h = today.getHours();
    let m = today.getMinutes();
    let s = today.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    
    // Set the content of the #timer div with the formatted time
    document.getElementById('timer').innerHTML = h + ":" + m + ":" + s;
    
    setTimeout(startTime, 1000);
  }
  
  function checkTime(i) {
    if (i < 10) {
      i = "0" + i;
    }
    return i;
  }
  
  // Function to open a new window with the current URL
  function openNewWindow() {
    const currentURL = window.location.href;
    const newURL = currentURL + '/LoginPage';
    const newWindow = window.open(newURL);
    newWindow.focus();
  }
</script>
  <button onclick="openNewWindow()">Open New Window</button>
</body>
</html>
)=====";


/*
 * Login page
 */

const char* LoginPageCode = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Login Page</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f0f0f0;
      margin: 20px;
    }
    .login-container {
      width: 60%;
      margin: 0 auto;
      padding: 20px;
      border: 1px solid #ccc;
      border-radius: 10px;
      background-color: #f9f9f9;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);
      height: 100%;
    }
    .login-container h2 {
      text-align: center;
      margin-bottom: 20px;
    }
    .login-container table {
      margin: 0 auto;
      border-collapse: collapse;
      width: 100%;
    }
    .login-container table th, .login-container table td {
      border: 1px solid #ccc;
      padding: 10px;
      text-align: center;
    }
    .login-container input[type='text'], .login-container input[type='password'] {
      width: 90%;
      padding: 10px;
      border: 1px solid #ccc;
      border-radius: 5px;
      margin-bottom: 10px;
    }
    .login-container input[type='submit'] {
      background-color: #4CAF50;
      color: white;
      padding: 10px 15px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      width: 50%;
    }
    .login-container input[type='submit']:hover {
      background-color: #45a049;
    }
    .login-container center {
      font-size: 18px;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .login-container .error-message {
      color: #ff0000;
      text-align: center;
      margin-top: 10px;
    }
  </style>
</head>
<body>
  <div class="login-container">
    <h2>ESP32 Login Page</h2>
    <form name='loginForm'>
      <table>
        <tr>
          <td colspan=2>
            <center><font size=4>Please log in to access ESP32 webserver reprogramming.</font></center>
          </td>
        </tr>
        <tr>
          <td>Username:</td>
          <td><input type='text' size=25 name='userid' placeholder='Enter your username'><br></td>
        </tr>
        <tr>
          <td>Password:</td>
          <td><input type='password' size=25 name='pwd' placeholder='Enter your password'><br></td>
        </tr>
        <tr>
          <td colspan="2">
            <input type='submit' onclick='check(this.form)' value='Login'>
          </td>
        </tr>
      </table>
    </form>
    <div class="error-message" id="errorMessage"></div>
  </div>
  <script>
    function check(form) {
      const username = form.userid.value;
      const password = form.pwd.value;
      if (username === 'admin' && password === 'admin') {
        window.open('/UpdatePage');
      } else {
        document.getElementById('errorMessage').innerHTML = 'Error: Invalid username or password';
      }
    }
  </script>
</body>
</html>
)=====";

/*
 * Control page
 */

const char* ControlPageCode = R"=====(
<html>
<head>
<!-- <meta http-equiv="refresh" content="5"> -->
<style>
  body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 20px; }
  form { margin-top: 10px; }
  input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; cursor: pointer; }
  .outer-container {
    display: flex;
    justify-content: center;
    align-items: center;
/*     width: 100vw; */
/*     height: 100vh; Set the height of the container to the full viewport height */
  }
  .control-container { background-color: #ffffff; border-radius: 20px; padding: 10px; margin-bottom: 10px; text-align: center; }
  .control-title { margin-top: 5px; margin-bottom: 5px; }
  .control-button { background-color: #4CAF50; color: white; padding: 8px 12px; border: none; cursor: pointer; margin-right: 10px;  margin-bottom: -10px;}
  .control-button.sensor-off { background-color: #f44336;margin-right: -5px; }
  .update-section {
    margin-top: 20px;
    margin-bottom: 20px;
/*     font-size: 20px; */
    border: 1px solid #ccc;
    padding: 10px;
    background-color: #f0f0f0;
    border-radius: 20px;
/*     white-space: pre-wrap; */
    word-wrap: break-word;
/*     max-width: 500px; */
    width: 75%;
  }
  #prg { font-size: 16px; }
  #timer {
    position: absolute;
    top: 0;
    right: 0;
    background-color: #000;
    color: #fff;
    padding: 5px;
    font-size: 14px;
  }
</style>
</head>
<body onload="startTime()">
<div id="timer"></div>
<div class="outer-container">
  <div class='control-container' id='sensor-container-1' style='width: 300px; text-align: center'>
    <h2 class='control-title'>Sensor Control</h2>
    <table style='margin: auto;'>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>DHT11</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='DHTstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='DHTstate' value='off'>Turn off</button>
    </form></th>
      </tr>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>TEMT6000</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='TEMTstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='TEMTstate' value='off'>Turn off</button>
    </form></th>
      </tr>
      <tr>
        <th style='padding: 8px; text-align: center;'>
          <div style='display: inline-block;'>PIR</div></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button' type='submit' name='PIRstate' value='on'>Turn on</button></form></th>
        <th style='text-align: center;'><form method='get'>
      		<button class='control-button sensor-off' type='submit' name='PIRstate' value='off'>Turn off</button>
    </form></th>
      </tr>
    </table>
  </div>
</div>
<div class="outer-container">
  <div class='control-container' id='sensor-container-1' style='width: 190px;'>
    <h2 class='control-title'>Load Control</h2>
    <form method='get'>
      <button class='control-button' type='submit' name='pinLoad' value='on'>Turn on</button>
      <button class='control-button sensor-off' type='submit' name='pinLoad' value='off'>Turn off</button>
    </form>
  </div>
</div>
<script>
$('form').submit(function(e){
e.preventDefault();
var form = $('#upload_form')[0];
var data = new FormData(form);
$.ajax({
url: '/update',
type: 'POST',
data: data,
contentType: false,
processData:false,
xhr: function() {
var xhr = new window.XMLHttpRequest();
xhr.upload.addEventListener('progress', function(evt) {
if (evt.lengthComputable) {
var per = evt.loaded / evt.total;
$('#prg').html('progress: ' + Math.round(per*100) + '%');

}, false);
return xhr;
},
success:function(d, s) {
console.log('success!');
},
error: function (a, b, c) {
}
});
}
</script>
<script>
  function startTime() {
    const today = new Date();
    let h = today.getHours();
    let m = today.getMinutes();
    let s = today.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    
    // Set the content of the #timer div with the formatted time
    document.getElementById('timer').innerHTML = h + ":" + m + ":" + s;
    
    setTimeout(startTime, 1000);
  }
  
  function checkTime(i) {
    if (i < 10) {
      i = "0" + i;
    }
    return i;
  }
  
  // Function to open a new window with the current URL
  function openUpdatePage() {
    window.open('/LoginPage');
  }
  function openSensorsPage() {
    window.open('/SensorsPage');
  }
</script>
<div class="outer-container">
    <button class="control-button" onclick="openUpdatePage()">Update code</button>
    <button class="control-button" onclick="openSensorsPage()">Sensor readings</button>
</div>
</body>
</html>
)=====";

const char* UpdatePageCode = R"=====(
<!DOCTYPE html>
<html>
<head>
<style>
  body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 20px; }
  form { margin-top: 10px; }
  input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 15px; border: none; cursor: pointer; }
  .outer-container {
    display: flex;
    justify-content: center;
    align-items: center;
/*     width: 100vw; */
/*     height: 100vh; Set the height of the container to the full viewport height */
  }
  .control-container { background-color: #ffffff; border-radius: 20px; padding: 10px; margin-bottom: 10px; text-align: center; }
  .control-title { margin-top: 5px; margin-bottom: 5px; }
  .control-button { background-color: #4CAF50; color: white; padding: 8px 12px; border: none; cursor: pointer; margin-right: 10px;  margin-bottom: -10px;}
  .control-button.sensor-off { background-color: #f44336;margin-right: -5px; }
  .update-section {
    margin-top: 20px;
    margin-bottom: 20px;
/*     font-size: 20px; */
    border: 1px solid #ccc;
    padding: 10px;
    background-color: #f0f0f0;
    border-radius: 20px;
/*     white-space: pre-wrap; */
    word-wrap: break-word;
/*     max-width: 500px; */
    width: 75%;
  }
  #prg { font-size: 16px; }
  #timer {
    position: absolute;
    top: 0;
    right: 0;
    background-color: #000;
    color: #fff;
    padding: 5px;
    font-size: 14px;
  }
</style>
</head>
<body onload="startTime()">
<div id="timer"></div>
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
<div class="outer-container">
  <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
    <input type='file' name='update'>
    <input type='submit' value='Update'>
  </form>
  <div class="update-section">
    <div id='prg'>Progress: 0%</div>
  </div>
</div>
<script>
$('form').submit(function(e){
e.preventDefault();
var form = $('#upload_form')[0];
var data = new FormData(form);
$.ajax({
url: "/update",
type: 'POST',
data: data,
contentType: false,
processData:false,
xhr: function() {
var xhr = new window.XMLHttpRequest();
xhr.upload.addEventListener('progress', function(evt) {
if (evt.lengthComputable) {
var per = evt.loaded / evt.total;
$('#prg').html('progress: ' + Math.round(per*100) + '%');
}
}, false);
return xhr;
},
success:function(d, s) {
console.log('success!');
},
error: function (a, b, c) {
}
});
});
</script>
<script>
  function startTime() {
    const today = new Date();
    let h = today.getHours();
    let m = today.getMinutes();
    let s = today.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    
    // Set the content of the #timer div with the formatted time
    document.getElementById('timer').innerHTML = h + ":" + m + ":" + s;
    
    setTimeout(startTime, 1000);
  }
  
  function checkTime(i) {
    if (i < 10) {
      i = "0" + i;
    }
    return i;
  }
</script>
</body>
</html>
)=====";

const char* SensorsPageCode = R"=====(
<!DOCTYPE html>
<html>
<title>Monitoring Page</title>
<head
<meta http-equiv="refresh" content="5">
<style>
  body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 20px; }
  .outer-container {
    display: flex;
    justify-content: center;
    align-items: center;
    width: 100%; 
  }
  .readings-section {
    font-size: 20px;
    border: 1px solid #ccc;
    padding: 10px;
    background-color: #f0f0f0;
    border-radius: 20px;
    word-wrap: break-word;
    width: 90%;
    margin-bottom: 50px;
  }
  #timer {
    position: absolute;
    top: 0;
    right: 0;
    background-color: #000;
    color: #fff;
    padding: 5px;
    font-size: 16px;
  }
  .control-button {
  	background-color: #4CAF50;
    color: white;
    padding: 10px 15px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 18px;
    font-weight: bold;
    box-shadow: 0 4px #2E8B57;
  }
  .control-button:hover {
    background-color: #45a049;
    box-shadow: 0 2px #2E8B57;
    transform: translateY(2px);
  }
</style>
</head>
<body onload="startTime()">
<div id="timer"></div>
<div class="outer-container">
  <div class='readings-section' id='readings' style='text-align: center;'>
    <h2>Sensor Readings</h2>
    <table style='margin: auto; border-collapse: collapse; width: 90%;'>
      <tr>
        <th style='border: 1px solid black; padding: 8px;'>Sensor</th>
        <th style='border: 1px solid black; padding: 8px;'>Value</th>
      </tr> 
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>MAX17043</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%MAX17043_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>DHT11</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%DHT11_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>TEMT6000</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%TEMT6000_VALUE%</td>
      </tr>
      <tr>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>PIR</td>
        <td style='border: 1px solid black; padding: 8px; text-align: center;'>%PIR_VALUE%</td>
      </tr>
    </table>
    <br>
  </div>
</div>

<table style='margin: auto; border-collapse: collapse; width: 90%;'>
  <!-- ... (existing table rows) ... -->
  <tr>
  <td style='border: 1px solid black; padding: 8px; text-align: center;'>Mode 1: Security mode</td>
  <td id='mode1-value' style='border: 1px solid black; padding: 8px; text-align: center;'></td>
</tr>
<tr>
  <td style='border: 1px solid black; padding: 8px; text-align: center;'>Mode 2: Battery discharge during sunlight</td>
  <td id='mode2-value' style='border: 1px solid black; padding: 8px; text-align: center;'></td>
</tr>
<tr>
  <td style='border: 1px solid black; padding: 8px; text-align: center;'>Mode 3: Battery charge during sunlight</td>
  <td id='mode3-value' style='border: 1px solid black; padding: 8px; text-align: center;'></td>
</tr>
<tr>
  <td style='border: 1px solid black; padding: 8px; text-align: center;'>Mode 4: Full operation mode</td>
  <td id='mode4-value' style='border: 1px solid black; padding: 8px; text-align: center;'></td>
</tr>
<tr>
  <td style='border: 1px solid black; padding: 8px; text-align: center;'>Mode 5: Full battery</td>
  <td id='mode5-value' style='border: 1px solid black; padding: 8px; text-align: center;'></td>
</tr>
</table>

<script>
  function startTime() {
    const today = new Date();
    let h = today.getHours();
    let m = today.getMinutes();
    let s = today.getSeconds();
    m = checkTime(m);
    s = checkTime(s);
    
    // Set the content of the #timer div with the formatted time
    document.getElementById('timer').innerHTML = h + ":" + m + ":" + s;
    
    setTimeout(startTime, 1000);
  }
  
  function checkTime(i) {
    if (i < 10) {
      i = "0" + i;
    }
    return i;
  }

  // Get the URL parameters using JavaScript
  const urlParams = new URLSearchParams(window.location.search);
  const mode1 = urlParams.get('mode1') === 'true';
  const mode2 = urlParams.get('mode2') === 'true';
  const mode3 = urlParams.get('mode3') === 'true';
  const mode4 = urlParams.get('mode4') === 'true';
  const mode5 = urlParams.get('mode5') === 'true';

  // Display the mode values on the page
  document.getElementById('mode1-value').innerText = mode1 ? 'true' : 'false';
  document.getElementById('mode2-value').innerText = mode2 ? 'true' : 'false';
  document.getElementById('mode3-value').innerText = mode3 ? 'true' : 'false';
  document.getElementById('mode4-value').innerText = mode4 ? 'true' : 'false';
  document.getElementById('mode5-value').innerText = mode5 ? 'true' : 'false';
  }
  
</script>
</body>
</html>
)=====";


const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";