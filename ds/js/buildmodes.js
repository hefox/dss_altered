// $Id: buildmodes.js,v 1.1.2.1 2009/10/24 13:32:28 swentel Exp $

Drupal.behaviors.ds_buildmodes = function(context) {

	$('.block-all').bind('click', function() {
    var excluder = this;
    $(excluder).parents('tr').find('.checkbox-instance').not('.block-all').each(function() {
      if(excluder.checked) {
        $(this).attr('disabled', 'disabled');
      }
      else {
        $(this).attr('disabled', '');
      }
    });
  });
}