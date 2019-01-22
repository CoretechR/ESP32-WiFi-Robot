static const char canvas_htm[] PROGMEM = "<!--This code is based on a project by Seb Lee-Delisle: http://seb.ly/2011/04/multi-touch-game-controller-in-javascripthtml5-for-ipad/-->\n"\
"<!doctype html>\n"\
"<html lang=en style=\"padding-bottom:80px\">\n"\
"  <head>\n"\
"    <meta charset=utf-8>\n"\
"    <meta name=\"viewport\" content=\"width=device-width, height=device-height, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\" />\n"\
"    <meta name=\"mobile-web-app-capable\" content=\"yes\">\n"\
"    <title>ESP32 Touch Control</title>\n"\
"    <style type=\"text/css\"> \n"\
"* {\n"\
"  -webkit-touch-callout: none; /* prevent callout to copy image, etc when tap to hold */\n"\
"  -webkit-text-size-adjust: none; /* prevent webkit from resizing text to fit */\n"\
"  /* make transparent link selection, adjust last value opacity 0 to 1.0 */\n"\
"  -webkit-tap-highlight-color: rgba(0,0,0,0); \n"\
"  -webkit-user-select: none; /* prevent copy paste, to allow, change 'none' to 'text' */\n"\
"  -webkit-tap-highlight-color: rgba(0,0,0,0); \n"\
"}\n"\
"body {\n"\
"  background-color: #000000;\n"\
"  margin: 0px;\n"\
"}\n"\
"canvas {\n"\
"  display:block; \n"\
"  position:absolute; \n"\
"  z-index: 1;\n"\
"}\n"\
".container {\n"\
"  width:auto;\n"\
"  text-align:center;\n"\
"  background-color:#ff0000;\n"\
"}\n"\
"\n"\
".button{\n"\
"  cursor: pointer;\n"\
"  border-radius: 10%;\n"\
"  border: 2px;\n"\
"  border-style: solid;\n"\
"  border-color: white;\n"\
"  text-align:center;\n"\
"  margin: 8px;\n"\
"  padding: 2px;\n"\
"}\n"\
"\n"\
".button light{\n"\
"  transition: background 0.4s;\n"\
"}\n"\
"\n"\
"input[type=\"checkbox\"]:checked + label {\n"\
"    background: gray;\n"\
"}\n"\
"\n"\
".sidenav {\n"\
"  overflow: hidden;\n"\
"  color: white;\n"\
"  //background-color: green;\n"\
"  position: fixed;\n"\
"  font-family: 'Roboto', sans-serif;\n"\
"  \n"\
"  width: 100%;\n"\
"  display: flex;\n"\
"  justify-content: space-between;  \n"\
"}\n"\
"canvas#stream{\n"\
"  //display: block;\n"\
"  margin: 0px;\n"\
"  //width: 100%;\n"\
"  height: auto;\n"\
"  //position:absolute;\n"\
"  //top:6vh;right:0;left:0;\n"\
"  margin:auto;\n"\
"  background:blue\n"\
"}\n"\
"    </style>\n"\
"    <link href=\"https://fonts.googleapis.com/css?family=Roboto\" rel=\"stylesheet\">\n"\
"  </head>\n"\
"  <!--\n"\
"    <ul id=\"messages\"></ul>\n"\
"    \n"\
"    <center>\n"\
"    <canvas id=\"stream\" width=\"160\" height=\"120\" style=\"z-index: -1;\"></canvas>\n"\
"    </center>\n"\
"    -->\n"\
"  <body scroll=\"no\" style=\"overflow: hidden; height:100vh;\">\n"\
"    \n"\
"    <script>\n"\
"//----------------OV7670 stuff-------------------------\n"\
"\n"\
"var r = 0;\n"\
"var capturecount = 1;\n"\
"var ln = 0;\n"\
"var flag = 0;\n"\
"var xres = 160;\n"\
"var yres = 120;\n"\
"var canvas;\n"\
"var ctx;\n"\
"var imgData;\n"\
"var gcanvasid = \"canvas-QQ-VGA\";    \n"\
"var camera_ip = \"192.168.4.1\";\n"\
"\n"\
"//-----------------------------------------------------\n"\
"\n"\
"\n"\
"host = window.location.hostname;\n"\
"if(!host) host = '192.168.4.1'; //For debugging only \n"\
"var ipaddr= host + \":81\"; // websock port of ESP32, normally xx.xx.xx.xx:81\n"\
"var websock = new WebSocket('ws://' + ipaddr);\n"\
"websock.binaryType = 'arraybuffer';\n"\
"websock.onopen = function(evt) {\n"\
"  console.log('websock open');\n"\
"  websock.send(gcanvasid); // initiate stream\n"\
"  console.log(\"Sending: \" + gcanvasid);\n"\
"};\n"\
"\n"\
"websock.onclose = function(evt) {\n"\
"  console.log('websock close');\n"\
"  toStatus('websock close');\n"\
"};\n"\
"\n"\
"websock.onerror = function(evt) { console.log(evt); toStatus(evt) };\n"\
"function toStatus(txt) {document.getElementById('status').innerHTML=txt; }\n"\
"function toReply(txt) {document.getElementById('reply').innerHTML=txt; }\n"\
" \n"\
"\n"\
" \n"\
"\n"\
"\n"\
"var Vector2 = function (x,y) {\n"\
"  this.x = x || 0;\n"\
"  this.y = y || 0;\n"\
"};\n"\
"\n"\
"Vector2.prototype = {\n"\
"\n"\
"  reset: function ( x, y ) {\n"\
"  \n"\
"    this.x = x;\n"\
"    this.y = y;\n"\
"    return this;\n"\
"  },\n"\
"  \n"\
"  copyFrom : function (v) {\n"\
"    this.x = v.x;\n"\
"    this.y = v.y;\n"\
"  },\n"\
"  \n"\
"  plusEq : function (v) {\n"\
"    this.x+=v.x;\n"\
"    this.y+=v.y;\n"\
"    return this;\n"\
"  },\n"\
"  \n"\
"  minusEq : function (v) {\n"\
"    this.x-=v.x;\n"\
"    this.y-=v.y;\n"\
"    return this; \n"\
"  },\n"\
"  \n"\
"  equals : function (v) {\n"\
"    return((this.x==v.x)&&(this.y==v.x));\n"\
"  }\n"\
"\n"\
"};\n"\
"\n"\
"\n"\
"var canvas,\n"\
"  scanvas,\n"\
"  c, // c is the canvas' context 2D\n"\
"  container, \n"\
"  halfWidth, \n"\
"  halfHeight,\n"\
"  leftTouchID = -1, \n"\
"  leftTouchPos = new Vector2(0,0),\n"\
"  leftTouchStartPos = new Vector2(0,0),\n"\
"  leftVector = new Vector2(0,0);\n"\
"\n"\
"var temperature;\n"\
"var sendFlag = false;\n"\
"\n"\
"setupCanvas();\n"\
"\n"\
"var mouseX, \n"\
"  mouseY,\n"\
"  mouseDown = false,\n"\
"  touches = []; // array of touch vectors;\n"\
"  \n"\
"var headlight = 0;\n"\
"\n"\
"setInterval(draw, 1000/30); // draw app at 30fps\n"\
"\n"\
"setInterval(sendControls, 1000/20); // send control input at 20fps\n"\
"\n"\
"\n"\
"\n"\
"canvas.addEventListener( 'touchstart', onTouchStart, false );\n"\
"canvas.addEventListener( 'touchmove', onTouchMove, false );\n"\
"canvas.addEventListener( 'touchend', onTouchEnd, false );\n"\
"window.onorientationchange = resetCanvas;  \n"\
"window.onresize = resetCanvas;  \n"\
"\n"\
"\n"\
"canvas.addEventListener( 'mousemove', onMouseMove, false );\n"\
"canvas.addEventListener( 'mousedown', onMouseDown, false );\n"\
"canvas.addEventListener( 'mouseup', onMouseUp, false );\n"\
"\n"\
"\n"\
"function resetCanvas (e) {\n"\
"  // resize the canvas - but remember - this clears the canvas too. \n"\
"  canvas.width = window.innerWidth; \n"\
"  canvas.height = window.innerHeight;\n"\
" \n"\
"  //halfWidth = canvas.width/2; \n"\
"  halfWidth = canvas.width;\n"\
"\n"\
"  halfHeight = canvas.height/2;\n"\
" \n"\
"  //make sure we scroll to the top left. \n"\
"  window.scrollTo(0,0); \n"\
"}\n"\
"\n"\
"var rawLeft, rawRight, MaxJoy = 255, MinJoy = -255, MaxValue = 255,\n"\
"  MinValue = -255, RawLeft, RawRight, ValLeft, ValRight;\n"\
"var leftMot = 0, rightMot = 0;\n"\
"\n"\
"function Remap(value, from1, to1, from2, to2){\n"\
"  return (value - from1) / (to1 - from1) * (to2 - from2) + from2;\n"\
"}\n"\
"\n"\
"//source: http://www.dyadica.co.uk/basic-differential-aka-tank-drive/\n"\
"function tankDrive(x, y){\n"\
"  \n"\
"  var z = Math.sqrt(x * x + y * y);\n"\
"  var rad = Math.acos(Math.abs(x) / z);\n"\
"  \n"\
"  if (isNaN(rad)) rad = 0;\n"\
"  var angle = rad * 180 / Math.PI;\n"\
"  var tcoeff = -1 + (angle / 90) * 2;\n"\
"  var turn = tcoeff * Math.abs(Math.abs(y) - Math.abs(x));\n"\
"  \n"\
"  turn = Math.round(turn * 100) / 100;\n"\
"  var move = Math.max(Math.abs(y), Math.abs(x));\n"\
"  \n"\
"  if ((x >= 0 && y >= 0) || (x < 0 && y < 0)){\n"\
"    rawLeft = move;\n"\
"    rawRight = turn;\n"\
"  }else{\n"\
"    rawRight = move;\n"\
"    rawLeft = turn;\n"\
"  }\n"\
"\n"\
"  if (y < 0){\n"\
"    rawLeft = 0 - rawLeft;\n"\
"    rawRight = 0 - rawRight;\n"\
"  }\n"\
"  \n"\
"  RawLeft = rawLeft;\n"\
"  RawRight = rawRight;\n"\
"  \n"\
"  leftMot = Remap(rawLeft, MinJoy, MaxJoy, MinValue, MaxValue);\n"\
"  rightMot = Remap(rawRight, MinJoy, MaxJoy, MinValue, MaxValue);\n"\
"}\n"\
"\n"\
"\n"\
"function draw() {\n"\
"  \n"\
"  c.clearRect(0,0,canvas.width, canvas.height); \n"\
"\n"\
"  //if touch\n"\
"  for(var i=0; i<touches.length; i++) {\n"\
"    \n"\
"    var touch = touches[i]; \n"\
"    \n"\
"    if(touch.identifier == leftTouchID){\n"\
"      c.beginPath(); \n"\
"      c.strokeStyle = \"white\"; \n"\
"      c.lineWidth = 6; \n"\
"      c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 40,0,Math.PI*2,true); \n"\
"      c.stroke();\n"\
"      c.beginPath(); \n"\
"      c.strokeStyle = \"white\"; \n"\
"      c.lineWidth = 2; \n"\
"      c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 60,0,Math.PI*2,true); \n"\
"      c.stroke();\n"\
"      c.beginPath(); \n"\
"      c.strokeStyle = \"white\"; \n"\
"      c.arc(leftTouchPos.x, leftTouchPos.y, 40, 0,Math.PI*2, true); \n"\
"      c.stroke(); \n"\
"      \n"\
"    } else {\n"\
"      \n"\
"      c.beginPath(); \n"\
"      c.fillStyle = \"white\";\n"\
"      //c.fillText(\"touch id : \"+touch.identifier+\" x:\"+touch.clientX+\" y:\"+touch.clientY, touch.clientX+30, touch.clientY-30); \n"\
"  \n"\
"      c.beginPath(); \n"\
"      c.strokeStyle = \"red\";\n"\
"      c.lineWidth = \"6\";\n"\
"      c.arc(touch.clientX, touch.clientY, 40, 0, Math.PI*2, true); \n"\
"      c.stroke();\n"\
"    }\n"\
"  }\n"\
"\n"\
"  //if not touch   \n"\
"  if(mouseDown){\n"\
"  \n"\
"    c.beginPath(); \n"\
"    c.strokeStyle = \"white\"; \n"\
"    c.lineWidth = 6; \n"\
"    c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 40,0,Math.PI*2,true); \n"\
"    c.stroke();\n"\
"    c.beginPath(); \n"\
"    c.strokeStyle = \"white\"; \n"\
"    c.lineWidth = 2; \n"\
"    c.arc(leftTouchStartPos.x, leftTouchStartPos.y, 60,0,Math.PI*2,true); \n"\
"    c.stroke();\n"\
"    c.beginPath(); \n"\
"    c.strokeStyle = \"white\"; \n"\
"    c.arc(leftTouchPos.x, leftTouchPos.y, 40, 0,Math.PI*2, true); \n"\
"    c.stroke(); \n"\
"        \n"\
"    c.fillStyle  = \"white\"; \n"\
"    //c.fillText(\"mouse : \"+mouseX+\", \"+mouseY, mouseX, mouseY); \n"\
"    c.beginPath(); \n"\
"    c.strokeStyle = \"white\";\n"\
"    c.lineWidth = \"6\";\n"\
"    c.arc(mouseX, mouseY, 40, 0, Math.PI*2, true); \n"\
"    c.stroke();\n"\
"  }\n"\
"\n"\
" \n"\
"  //c.fillText(\"Left Motor: \" + leftMot + \" Right Motor: \" + rightMot, 10, 30);\n"\
"  //c.fillText(\"Left Vx: \" + leftVector.x + \" Left Vy: \" + leftVector.y, 10, 50);\n"\
"  //c.fillText(\"Temperature: \"+temperature+\"&#176C\", 10, 30);\n"\
" \n"\
" \n"\
"  websock.onmessage=function(event) {\n"\
"  \n"\
"    console.log('Data received: ' + event.data);\n"\
"    //console.log(typeof event.data);\n"\
"    \n"\
"    if (typeof event.data === 'string'){\n"\
"      var msgReceived=JSON.parse(event.data);\n"\
"      console.log(msgReceived);\n"\
"      if(msgReceived.hasOwnProperty('temp')){\n"\
"        temperature = parseInt(msgReceived.temp);\n"\
"        document.getElementById(\"temp\").innerHTML = temperature + '&#176C';\n"\
"      }\n"\
"      if(msgReceived.hasOwnProperty('volt')){\n"\
"        voltage = parseFloat(msgReceived.volt).toFixed(2);\n"\
"        document.getElementById(\"volt\").innerHTML = voltage + 'V';\n"\
"      }\n"\
"    }\n"\
"    else{\n"\
"      var arraybuffer = event.data;\n"\
"      if (arraybuffer.byteLength == 1){\n"\
"        flag  = new Uint8Array(event.data); // Start Flag\n"\
"        if (flag == 0xAA) {\n"\
"          ln = 0;\n"\
"        }\n"\
"        if (flag == 0xFF) {\n"\
"          //alert(\"Last Block\");\n"\
"        }        \n"\
"        if (flag == 0x11) {\n"\
"          //alert(\"Camera IP\");\n"\
"        }\n"\
"        \n"\
"      } else{\n"\
"        \n"\
"        if (flag == 0x11) {\n"\
"          //alert(\"Camera IP \" + event.data);\n"\
"          camera_ip = event.data;\n"\
"          //document.getElementById(\"wifi-ip\").innerText = camera_ip;\n"\
"          flag = 0;\n"\
"        } else{\n"\
"          var bytearray = new Uint8Array(event.data);\n"\
"          display(bytearray, arraybuffer.byteLength, flag);\n"\
"        }\n"\
"      }\n"\
"    }\n"\
"    \n"\
"    \n"\
"  }\n"\
"  \n"\
"\n"\
"  \n"\
"  \n"\
"  \n"\
"  \n"\
"  \n"\
"}\n"\
"\n"\
"\n"\
"window.addEventListener('resize', resizeStream, false);\n"\
"\n"\
" function resizeStream() {\n"\
"   aspectRatio = 160/120;\n"\
"   maxHeight = window.innerHeight - document.getElementById('navTop').offsetHeight - document.getElementById('navBottom').offsetHeight;\n"\
"   if(window.innerWidth*aspectRatio <= maxHeight){\n"\
"     document.getElementById('stream').style.height = window.innerWidth*aspectRatio + \"px\";\n"\
"     document.getElementById('stream').style.width = window.innerWidth + \"px\";\n"\
"   }\n"\
"   else {\n"\
"     document.getElementById('stream').style.height = maxHeight + \"px\";\n"\
"     document.getElementById('stream').style.width = maxHeight/aspectRatio + \"px\";\n"\
"   }\n"\
" }\n"\
"\n"\
"\n"\
"\n"\
"\n"\
"\n"\
"\n"\
"function display(pixels, pixelcount, flag){\n"\
"\n"\
"  var i = 0;\n"\
"  var scanvas = document.getElementById('stream');\n"\
"  var ctx = scanvas.getContext('2d');\n"\
"  imgData = ctx.createImageData(120, 160);\n"\
"  \n"\
"  for(x=xres-1; x >= 0; x--){\n"\
"    for(y=0; y < yres; y++){\n"\
"	  \n"\
"      i = (y * xres + x) << 1;\n"\
"      pixel16 = (0xffff & pixels[i]) | ((0xffff & pixels[i+1]) << 8);\n"\
"      imgData.data[ln+0] = ((((pixel16 >> 11) & 0x1F) * 527) + 23) >> 6;\n"\
"      imgData.data[ln+1] = ((((pixel16 >> 5) & 0x3F) * 259) + 33) >> 6;\n"\
"      imgData.data[ln+2] = (((pixel16 & 0x1F) * 527) + 23) >> 6;  \n"\
"      imgData.data[ln+3] = (0xFFFFFFFF) & 255;\n"\
"      ln += 4;\n"\
"    }\n"\
"  }\n"\
"  \n"\
"  if (flag == 0xFF) { // last block\n"\
"    ln = 0;\n"\
"    \n"\
"	\n"\
"    ctx.putImageData(imgData,0,0);\n"\
"    websock.send(\"next-frame\");\n"\
"    console.log(\"Sending: next-frame\");\n"\
"  }\n"\
"    \n"\
"}\n"\
"\n"\
"/* \n"\
"* Touch event\n"\
"*/  \n"\
"\n"\
"function onTouchStart(e) {\n"\
"  \n"\
"  for(var i = 0; i<e.changedTouches.length; i++){\n"\
"    var touch =e.changedTouches[i];\n"\
"    //console.log(leftTouchID + \" \"\n"\
"    if((leftTouchID<0) && (touch.clientX<halfWidth)){\n"\
"	\n"\
"      leftTouchID = touch.identifier;\n"\
"      leftTouchStartPos.reset(touch.clientX, touch.clientY);\n"\
"      leftTouchPos.copyFrom(leftTouchStartPos);\n"\
"      leftVector.reset(0,0);\n"\
"      continue;\n"\
"    } else{\n"\
"	\n"\
"      makeBullet();\n"\
"    }\n"\
"  }\n"\
"  touches = e.touches;\n"\
"}\n"\
"\n"\
"function onMouseDown(event) {\n"\
"  leftTouchStartPos.reset(event.offsetX, event.offsetY);\n"\
"  leftTouchPos.copyFrom(leftTouchStartPos);\n"\
"  leftVector.reset(0,0);\n"\
"  mouseDown = true;\n"\
"}\n"\
" \n"\
"function onTouchMove(e) {\n"\
"  // Prevent the browser from doing its default thing (scroll, zoom)\n"\
"  e.preventDefault();\n"\
"  \n"\
"  for(var i = 0; i<e.changedTouches.length; i++){\n"\
"    var touch =e.changedTouches[i];\n"\
"    if(leftTouchID == touch.identifier){\n"\
"      leftTouchPos.reset(touch.clientX, touch.clientY);\n"\
"      leftVector.copyFrom(leftTouchPos);\n"\
"      leftVector.minusEq(leftTouchStartPos);\n"\
"      sendFlag = true;\n"\
"      break;\n"\
"    }\n"\
"  }\n"\
"  \n"\
"  touches = e.touches;\n"\
"  \n"\
"}\n"\
"\n"\
"function onMouseMove(event){\n"\
"  \n"\
"  mouseX = event.offsetX;\n"\
"  mouseY = event.offsetY;\n"\
"  if(mouseDown){\n"\
"    leftTouchPos.reset(event.offsetX, event.offsetY);\n"\
"    leftVector.copyFrom(leftTouchPos);\n"\
"    leftVector.minusEq(leftTouchStartPos);\n"\
"    sendFlag = true;\n"\
"  }\n"\
"  \n"\
"}\n"\
" \n"\
"function onTouchEnd(e){\n"\
"  \n"\
"  touches = e.touches;\n"\
"  \n"\
"  for(var i = 0; i<e.changedTouches.length; i++){\n"\
"    var touch =e.changedTouches[i];\n"\
"    if(leftTouchID == touch.identifier){\n"\
"      leftTouchID = -1;\n"\
"      leftVector.reset(0,0);\n"\
"      leftMot = rightMot = 0;\n"\
"      sendFlag = true;\n"\
"      break;\n"\
"    }\n"\
"  }\n"\
"  \n"\
"}\n"\
"\n"\
"function onMouseUp(event) { \n"\
"  \n"\
"  leftVector.reset(0,0);\n"\
"  leftMot = rightMot = 0;\n"\
"  mouseDown = false;\n"\
"  sendFlag = true;\n"\
"}\n"\
"\n"\
"/*\n"\
"Source for keyboard detection: Braden Best:\n"\
"https://stackoverflow.com/questions/5203407/how-to-detect-if-multiple-keys-are-pressed-at-once-using-javascript\n"\
"*/ \n"\
"var map = {};\n"\
"onkeydown = onkeyup = function(e){\n"\
"  e = e || event; // to deal with IE\n"\
"  map[e.keyCode] = e.type == 'keydown';\n"\
"  \n"\
"  if(map[38]){ // ArrowUp\n"\
"    leftVector.y = -55;\n"\
"  }\n"\
"  if(map[40]){ // ArrowDown\n"\
"    leftVector.y = 55;\n"\
"  }\n"\
"  if(map[37]){ // ArrowLeft\n"\
"    leftVector.x = -55;\n"\
"  }\n"\
"  if(map[39]){ // ArrowRight\n"\
"    leftVector.x = 55;\n"\
"  }\n"\
"  \n"\
"  if(!map[38] && !map[40]){ // ArrowUp/Down is not pressed\n"\
"    leftVector.y = 0;\n"\
"  }\n"\
"  if(!map[37] && !map[39]){ // ArrowLeft/Right is not pressed\n"\
"    leftVector.x = 0;\n"\
"  }\n"\
"  if(leftVector.y == 0 && leftVector.x == 0) leftMot = rightMot = 0;\n"\
"  sendFlag = true;\n"\
"}\n"\
"\n"\
"function setupCanvas() {\n"\
"  \n"\
"  canvas = document.createElement( 'canvas' );\n"\
"  c = canvas.getContext( '2d' );\n"\
"  container = document.createElement( 'div' );\n"\
"  container.className = \"container\";\n"\
"  \n"\
"  document.body.appendChild( container );\n"\
"  container.appendChild(canvas);\n"\
"  \n"\
"  resetCanvas();\n"\
"  \n"\
"  c.strokeStyle = \"#ffffff\";\n"\
"  c.lineWidth =2;\n"\
"}\n"\
"\n"\
"function mouseOver(minX, minY, maxX, maxY){\n"\
"  return(mouseX>minX&&mouseY>minY&&mouseX<maxX&&mouseY<maxY);\n"\
"}\n"\
"\n"\
"function sendControls(){\n"\
"  if(sendFlag == true){\n"\
"    leftVector.x = Math.min(Math.max(parseInt(leftVector.x), -255), 255);\n"\
"    leftVector.y = Math.min(Math.max(parseInt(leftVector.y), -255), 255);\n"\
"    \n"\
"    tankDrive(leftVector.x, -leftVector.y);\n"\
"    if(leftMot > 0) leftMot += 70;\n"\
"    if(leftMot < 0) leftMot -= 70;\n"\
"    if(rightMot > 0) rightMot += 70;\n"\
"    if(rightMot < 0) rightMot -= 70;\n"\
"    leftMot = Math.min(Math.max(parseInt(leftMot), -255), 255);\n"\
"    rightMot = Math.min(Math.max(parseInt(rightMot), -255), 255);\n"\
"    \n"\
"    websock.send(JSON.stringify({ cmd : \"pos\", val : leftMot, val2 : rightMot }));\n"\
"    sendFlag = false;\n"\
"  }\n"\
"}\n"\
"\n"\
"\n"\
"\n"\
"function takePicture(){\n"\
"  websock.send(JSON.stringify({ cmd : \"cam\", val : 1 }));\n"\
"}\n"\
"\n"\
"function shutdown(){\n"\
"  if(confirm(\"This will shutdown the ESP32.\\nAre you sure?\")){\n"\
"    websock.send(JSON.stringify({ cmd : \"power\", val : 1 }));\n"\
"  }\n"\
"}\n"\
"// Handle headlight button\n"\
"function handleHeadlightClick(cb){\n"\
"  if (headlight == 0){\n"\
"    headlight = 1;\n"\
"	document.getElementById(\"light\").style.backgroundColor = \"gray\";\n"\
"  } else{\n"\
"    headlight = 0;\n"\
"	document.getElementById(\"light\").style.backgroundColor = \"black\";\n"\
"	\n"\
"  }\n"\
"  websock.send(JSON.stringify({ cmd : \"light\", val : headlight }));\n"\
"}\n"\
"\n"\
"\n"\
"    </script>\n"\
"    <div id=\"navTop\" class=\"sidenav\"style=\"position:relative;\">\n"\
"      <a id=\"temp\" style=\"padding-left: 3%;\">0&#176C </a>\n"\
"      <a id=\"volt\" style=\"padding-right: 3%;\">0V</a>\n"\
"    </div>\n"\
"	\n"\
"	<div style=\"position:relative;z-index:0;\">\n"\
"      <canvas id=\"stream\" width=\"120\" height=\"160\"> </canvas>\n"\
"    </div>\n"\
"	\n"\
"    <div id=\"navBottom\" class=\"sidenav\" style=\"bottom:0;z-index: 2;\">\n"\
"      <a id=\"off\" class=\"button off\" type=\"button\" onclick=\"shutdown()\" style=\"\">OFF</a>\n"\
"      <a id=\"cam\" class=\"button cam\" type=\"button\" onclick=\"takePicture()\" style=\"\">CAM</a>\n"\
"	  <a id=\"light\" class=\"button light\" type=\"button\" onclick=\"handleHeadlightClick()\" style=\"transition: background 0.4s;\">LIGHT</a>\n"\
"	</div>\n"\
"    \n"\
"	<script>resizeStream();</script>\n"\
"  </body>\n"\
"</html>\n";
