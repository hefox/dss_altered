<?php
// $Id: node.tpl.php,v 1.1.2.6.2.2 2009/11/26 10:10:50 swentel Exp $
/**
 * This node template is optimized for use with the Node displays module.
 * Differences with the default node.tpl.php
 * - Uses only the $content variable.
 * - Extra check on $sticky because this node might be build with another build mode.
 *
 * This template is optional, you are free to use your own node template of course.
 * All other node variables are still available of course, for a list, take a look at
 * the node.tpl.php in modules/node.
 */
?>
<div class="node <?php if (isset($node_classes)) print $node_classes; ?><?php if ($is_front) { print ' front-node'; } ?><?php if ($sticky && $node->build_mode == 'sticky') { print ' sticky'; } ?><?php if (!$status) { print ' node-unpublished'; } ?> clear-block" id="node-<?php print $node->nid; ?>">
  <div class="node-inner">
    <div class="buildmode-<?php print $node->build_mode; ?>">
      <?php print $content; ?>
    </div>
  </div> <!-- /node-inner -->
</div> <!-- /node -->