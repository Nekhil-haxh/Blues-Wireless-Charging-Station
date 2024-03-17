#ifndef WEBPAGE_H
#define WEBPAGE_H

const char style[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Login Form</title>
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.4/css/all.min.css">
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@300;500;600&display=swap" rel="stylesheet">
    <script src="https://unpkg.com/@dotlottie/player-component@latest/dist/dotlottie-player.mjs" type="module"></script> 
    <style media="screen">
        *,
        *:before,
        *:after{
            padding: 0;
            margin: 0;
            box-sizing: border-box;
        }
        body{
            background-color: #080710;
        }
        .background{
            width: 430px;
            height: 520px;
            position: absolute;
            transform: translate(-50%,-50%);
            left: 50%;
            top: 50%;
        }
        .background .shape{
            height: 200px;
            width: 200px;
            position: absolute;
            border-radius: 50%;
        }
        .shape:first-child{
            background: linear-gradient(
                #1845ad,
                #23a2f6
            );
            left: -80px;
            top: -80px;
        }
        .shape:last-child{
            background: linear-gradient(
                to right,
                #ff512f,
                #f09819
            );
            right: -30px;
            bottom: -80px;
        }
        form{
            height: 520px;
            width: 400px;
            background-color: rgba(255,255,255,0.13);
            position: absolute;
            transform: translate(-50%,-50%);
            top: 50%;
            left: 50%;
            border-radius: 10px;
            backdrop-filter: blur(10px);
            border: 2px solid rgba(255,255,255,0.1);
            box-shadow: 0 0 40px rgba(8,7,16,0.6);
            padding: 50px 35px;
        }
        form *{
            font-family: 'Poppins',sans-serif;
            color: #ffffff;
            letter-spacing: 0.5px;
            outline: none;
            border: none;
        }
        form h3{
            font-size: 32px;
            font-weight: 500;
            line-height: 42px;
            text-align: center;
        }

        label{
            display: block;
            margin-top: 30px;
            font-size: 20px;
            font-weight: 500;
        }
        input{
            display: block;
            height: 50px;
            width: 100%;
            background-color: rgba(255,255,255,0.07);
            border-radius: 3px;
            padding: 0 10px;
            margin-top: 8px;
            font-size: 20px;
            font-weight: 400;
            padding-top: 0.75rem;
        }
        #credit{
            display: block;
            height: 50px;
            width: 100%;
            background-color: rgba(255,255,255,0.07);
            border-radius: 3px;
            padding: 0 10px;
            margin-top: 8px;
            font-size: 20px;
            font-weight: 400;
            padding-top: 0.75rem;
        }
        ::placeholder{
            color: #e5e5e5;
        }
        button{
            margin-top: 50px;
            width: 100%;
            background-color: #ffffff;
            color: #080710;
            padding: 15px 0;
            font-size: 22px;
            font-weight: 600;
            border-radius: 5px;
            cursor: pointer;
        }
        
        .lottie{
            height: 10rem;
            weight: 10rem;
            margin-top: 1rem;
            margin-left: -0.5rem;
        }

    </style>
</head>
)=====";

const char home[] PROGMEM = R"=====(
<body>
    <div class="background">
        <div class="shape"></div>
        <div class="shape"></div>
    </div>
    <form id="loginForm" action="/submit" method="POST">
        <h3>Login Here</h3>

        <label for="username">Username</label>
        <input type="text" placeholder="Email or Phone" id="username" name="username">

        <label for="password">Password</label>
        <input type="password" placeholder="Password" id="password" name="password">

        <p id="errorMessage" style="color: red;"></p>

        <button type="submit">Log In</button>
        
    </form>

    <script>
        function submitForm(event) {
            event.preventDefault(); // Prevent default form submission

            // Get form data
            var username = document.getElementById("username").value;
            var password = document.getElementById("password").value;

            // Create JSON object
            var formData = {
                "username": username,
                "password": password
            };

            // Convert JSON object to string
            var jsonData = JSON.stringify(formData);

            // Make an AJAX request to the server
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    // Handle response
                    console.log("Form submitted successfully");
                }
            };
            xhttp.open("POST", "/submit", true);
            xhttp.setRequestHeader("Content-Type", "application/json");
            xhttp.send(jsonData);
        }
    </script>
</body>
</html>
)=====";

const char start[] PROGMEM = R"=====(
<body>
    <div class="background">
        <div class="shape"></div>
        <div class="shape"></div>
    </div>
    <form id="loginForm" action="/start" method="POST">
        <h3>Welcome {{username}}</h3>

        <label for="username">Credits Left: </label>
        <div id="credit">
            {{credit}}
        </div>

        <button type="submit">Start Charging</button>
        
    </form>
</body>
</html>
)=====";

const char progress[] PROGMEM = R"=====(
<body>
    <div class="background">
        <div class="shape"></div>
        <div class="shape"></div>
    </div>
    
    <form id="loginForm" action="/stop" method="POST">
        <h3>Welcome </h3>
        
        <label>Charging In Progress...</label>
        
        <button type="submit">Stop Charging</button>
    </form>
</body>
</html>
)=====";

const char stop[] PROGMEM = R"=====(
<body>
    <div class="background">
        <div class="shape"></div>
        <div class="shape"></div>
    </div>
    <form id="loginForm" action="/start" method="POST">
        <h3>Welcome {{username}}</h3>

        <label for="username">Credits Left: </label>
        <div id="credit">
            {{credit}}
        </div>

        <button type="submit">Start Charging</button>
        <button id="logoutBtn">Logout</button>
    </form>

    <script>
        document.getElementById("logoutBtn").addEventListener("click", function(){
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/logout");
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                if (xhr.status === 200) {
                    window.location.href = "/";
                } else {
                    console.error(xhr.responseText);
                }
            }
        };
        xhr.send();
    });
  </script>
</body>
</html>
)=====";
#endif
