function clickevent(){
  setCookie('adult','Agree');
}

function setCookie(c_name,value)
{
  var c_value=escape(value);
  document.cookie=c_name + "=" + c_value;
}



