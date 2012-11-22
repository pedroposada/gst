// show hide admin link
function prepareAdminLinks() {
  jQuery('ul#om-menu-user-menu ul.menu').prepend('<li><a id="adminLinkToggle" class="admin-links-active" href="#">Hide Admin Blocks</a></li>');
  jQuery('a#adminLinkToggle').click(function(e){
      e.preventDefault();
      if (jQuery('a#adminLinkToggle').hasClass('admin-links-active')) {      
        jQuery('div#content-tabs').slideUp('fast');
        jQuery('div.contextual-links-wrapper').slideUp('fast');
        jQuery('fieldset.filter-wrapper').slideUp('fast');
				jQuery('div#content-messages').slideUp('fast');
        jQuery('a#adminLinkToggle').text('Show Admin Blocks');        
      } else {
        jQuery('div#content-tabs').slideDown('fast');
        jQuery('div.contextual-links-wrapper').slideDown('fast');
        jQuery('fieldset.filter-wrapper').slideDown('fast');
				jQuery('div#content-messages').slideDown('fast');
        jQuery('a#adminLinkToggle').text('Hide Admin Blocks');  
      }
      jQuery('a#adminLinkToggle').toggleClass('admin-links-active');    
  });
}
