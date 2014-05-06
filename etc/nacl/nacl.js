var module    = null;
var hudArea   = null;
var hudText   = null;
var hasLoaded = false;
var hasEnded  = false;

window.onload   = onLoad;
window.onresize = onResize;

function handleMessage( e )
{
  var type = e.data.substring( 0, 5 );
  var text = e.data.substring( 5 );

  if( type == "quit:" ) {
    hasEnded = true;

    window.onbeforeunload = null;
    module.style.visibility = "hidden";

    hudArea.style.display = "inline";
    hudText.innerHTML = MSG_FINISHED;
  }
  else if( type == "none:" ) {
    hudArea.style.display = "none";
    hudText.innerHTML = "";
  }
  else if( type == "init:" ) {
    window.onbeforeunload = onBeforeUnload;

    hudArea.style.display = "inline";
    hudText.innerHTML = MSG_INITIALISING;
  }
  else if( type == "upd0:" ) {
    hudArea.style.display = "inline";
    hudText.innerHTML = MSG_CHECKING;
  }
  else if( type == "upd1:" ) {
    hudArea.style.display = "inline";
    hudText.innerHTML = MSG_DOWNLOADING + text;
  }
  else if( type == "data:" ) {
    hudArea.style.display = "inline";
    hudText.innerHTML = MSG_READING + text;
  }
  else if( type == "lang:" ) {
    module.postMessage( "lang:" + navigator.language );
  }
  else if( type == "navi:" ) {
    window.open( text, "_blank" );
  }
}

function updateLoadProgress()
{
  hasLoaded = true;

  if( event.lengthComputable && event.total > 0 ) {
    var percent = Math.round( event.loaded / event.total * 100.0 );

    hudText.innerHTML = MSG_LOADING + "<br/>" + percent + " %";
  }
  else {
    hudText.innerHTML = MSG_LOADING;
  }
}

function onLoadEnd( e )
{
  hasLoaded = true;

  hudText.innerHTML = MSG_LAUNCHING;
}

function onResize()
{
  if( hasLoaded && hasEnded ) {
    module.style.visibility = "hidden";
  }
}

function onLoad()
{
  module  = document.getElementById( "nacl" );
  hudArea = document.getElementById( "hudArea" );
  hudText = document.getElementById( "hudText" );

  module.addEventListener( "message", handleMessage, true );
  module.addEventListener( "progress", updateLoadProgress, true );
  module.addEventListener( "loadend", onLoadEnd, true );
}

function onBeforeUnload()
{
  module.postMessage( "quit:" );

  return MSG_CLOSE;
}
