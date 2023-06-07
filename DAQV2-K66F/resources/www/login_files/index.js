$("#login-button").click(function(event){
		 event.preventDefault();
	 }		
	 $('form').fadeOut(500);
	$('.wrapper').addClass('form-success');
});
		function checklogin()
		{
			if (form.username.value=="admin") { 
				if (form.password.value=="admin") {
					window.location = "testmenu.html";
				}
			}				
		}

		function checklogin1()
		{
			window.location = "testmenu.html";			
		}