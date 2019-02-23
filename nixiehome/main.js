var request = require('request');
var firebase = require('firebase');
var exec = require('child_process').exec;

var posturl =  'http://192.168.179.7/';

var config = {
        apiKey: "AIzaSyAklPH-tgdEIGA4xEhvNXTH94G1hiFT9fE",
        authDomain: "dappun-2843f.firebaseapp.com",
        databaseURL: "https://dappun-2843f.firebaseio.com",
        projectId: "dappun-2843f",
        storageBucket: "dappun-2843f.appspot.com",
        messagingSenderId: "122101807315"
      };

//firebaseのあれこれ
firebase.initializeApp(config);
var ref = firebase.database().ref("/mode");

var sendval = 0;

ref.on('value', snapshot => {
    var mode = snapshot.val().replace(/\s+/g, "");
    console.log("mode : " + mode);

    switch(mode){
        case "時刻":
            sendval = 0;
            break;
        case "日付":
            sendval = 1;
            break;
        case "温度":
            sendval = 2;
            break;
        case "湿度":
            sendval = 2;
            break;
        case "気圧":
            sendval = 3;
            break;
        case "GPS":
            sendval = 0;
            break;
        case "カスタム":
            sendval = 4;
            break;
        default :
            sendval = 0;
            break;
    }
    console.log(sendval);

    exec('curl ' + posturl + 'setting?mode=' + sendval, function(err, stdout, stderr){

    });
});
