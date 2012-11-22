jQuery( function($) {
  $(document).ready(function() {
   $("form#views-exposed-form-back-issues-page select").change(function() {
      $("form#views-exposed-form-back-issues-page").submit();
    });
  });
}); 

