<?php
/**
 * local function, dependency : simplehtmldom module
 * @description: 
 */
function _get_eg_item_simplehtmldom($bib) {
	$hosturl = 'https://catalog.berklee.edu'; $bibdir = '/eg/opac/record/'; $imgurl = '/opac/extras/ac/jacket/medium/r/';
	$biburl = $hosturl.$bibdir.$bib;
	$imgurl = $hosturl.$imgurl.$bib;
	$html_obj = new simple_html_dom();
	$html_obj->load_file($biburl);  
	/*
		foreach($html_obj->find('img[id=rdetail_image]') as $element) {
		   dsm($element->src);
		}
	*/
	
	$title = $html_obj->find('h1[id=rdetail_title]', 0)->plaintext;	    
	//dsm($title);
	$author = $html_obj->find('span.rdetail-author-div', 0)->plaintext;	    
	//dsm($author);
	$type = trim($html_obj->find('div[id=rdetail_format_label]', 0)->plaintext);
	//dsm($type);
	//$imgsrc = $html_obj->find('img[id=rdetail_image]', 0)->src;
	//$imgurl = $hosturl.$imgsrc;
	//$callnumber = $html_obj->find('td[headers=copy_header_callnumber]', 0)->plaintext;   
	//dsm($callnumber);
	//$barcode = $html_obj->find('td[headers=copy_header_barcode]', 0)->plaintext;   
	//dsm($barcode);
	//dsm('--------------------------------------------------------------------------');
	
	//dsm($html_obj->find('div[id=rdetail_image_div]'));
	//dsm($html_obj);
	//form_set_error('time', t('-------------------------------------'));
	//http://placehold.it/200&text=Jacket%20image%20not%20avaiable
	
	$imgurl = _check_url($imgurl) == 200 ? $imgurl : 'http://placehold.it/200&text=Jacket%20image%20not%20avaiable';
	return array('type'=>$type, 'title'=>$title, 'author'=>$author, 'img'=>$imgurl, 'url'=>$biburl);
}
/**
 * local function
 * @description: 
 */
function _get_eg_item_rss2($bib) {
	$hosturl = 'https://catalog.berklee.edu'; $imgparams = '/opac/extras/ac/jacket/medium/r/';
	$bibparams = '/opac/extras/supercat/retrieve/rss2/record/';
	$biburl = $hosturl.$bibparams.$bib;
	$imgurl = $hosturl.$imgparams.$bib;
	//$xmlObj = json_decode(json_encode((array) simplexml_load_file($biburl)->channel->item), 1);
	$xmlObj = simplexml_load_file($biburl)->channel->item;
	$ns = $xmlObj->getNamespaces(true);
	$dc = $xmlObj->children($ns['dc']);
	
	$title = (string)$xmlObj->title;
	$author = preg_replace("/\(berklee.*?\d{1,}/", "", (string)(isset($xmlObj->author) ? $xmlObj->author : $dc->creator[0]));
	$type = (string)$dc->type[0];
	$bibrss = array(); preg_match("/\/([^\)]+)\//", (string)$xmlObj->guid, $bibrss);
	
	$htmllink = $hosturl.'/eg/opac/record/'.$bib;
	//$imgurl = _check_url($imgurl) == 200 ? $imgurl : 'http://placehold.it/200&text=image%20not%20avaiable%20for%20'.$title;
	$imgurl = _check_url($imgurl) == 200 ? $imgurl : 'http://placehold.it/200&text='.$type;
	return array('type'=>$type, 'title'=>$title, 'author'=>$author, 'img'=>$imgurl, 'url'=>$htmllink);
}
/**
 * local function
 * @description: 
 */
function _get_eg_item_marcxml($bib) {
	$hosturl = 'https://catalog.berklee.edu'; $imgparams = '/opac/extras/ac/jacket/medium/r/'; $largeimgparams = '/opac/extras/ac/jacket/large/r/';
	$bibparams = '/opac/extras/supercat/retrieve/marcxml-full/record/';
	$biburl = $hosturl.$bibparams.$bib;
	$imgurl = $hosturl.$imgparams.$bib;
	$largeimgurl = $hosturl.$largeimgparams.$bib;
	$marcarray = json_decode(json_encode(simplexml_load_file($biburl)->record), 1);
	//$xmlObj = simplexml_load_file($biburl)->record;
	$marcinfo=array();
	foreach($marcarray['datafield'] as $each){
		switch($each['@attributes']['tag']){
			//case (100): $marcinfo['author'] = strpos($each['subfield'][0], '(berklee)') == false ? $each['subfield'][0] : $each['subfield'][1]; break;
			//case (110): $marcinfo['author'] = strpos($each['subfield'][0], '(berklee)') == false ? $each['subfield'][0] : $each['subfield'][1]; break;
			case (100): $marcinfo['author'] = $each['subfield']; break;
			case (110): $marcinfo['author'] = $each['subfield']; break;
			case (245): $marcinfo['title'] = $each['subfield']; break;
		}
	}
	
	if(isset($marcarray['holdings']['volumes']['volume'][0]['@attributes']['label']))
	{
		$marcinfo['callnumber'] = $marcarray['holdings']['volumes']['volume'][0]['@attributes']['label'];
		$marcinfo['type'] = isset($marcarray['holdings']['volumes']['volume'][0]['copies']['copy'][0]) ? $marcarray['holdings']['volumes']['volume'][0]['copies']['copy'][0]['@attributes']['circ_modifier'] : $marcarray['holdings']['volumes']['volume'][0]['copies']['copy']['@attributes']['circ_modifier'];
	}
	else
	{
		$marcinfo['callnumber'] = $marcarray['holdings']['volumes']['volume']['@attributes']['label'];
		$marcinfo['type'] = isset($marcarray['holdings']['volumes']['volume']['copies']['copy'][0]) ? $marcarray['holdings']['volumes']['volume']['copies']['copy'][0]['@attributes']['circ_modifier'] : $marcarray['holdings']['volumes']['volume']['copies']['copy']['@attributes']['circ_modifier'];
	}
	
	//dsm($bib);
	//dsm($marcarray['holdings']['volumes']);	
	
	$marcinfo['title'] = implode(' ',$marcinfo['title']);
	$marcinfo['author'] = preg_replace("/\(berklee.*?\d{1,}/", "", implode(' ',$marcinfo['author']));
	$marcinfo['type'] = strtoupper($marcinfo['type']);
	
	$marcinfo['url'] = $hosturl.'/eg/opac/record/'.$bib;
	//$imgurl = _check_url($imgurl) == 200 ? $imgurl : 'http://placehold.it/200&text=image%20not%20avaiable%20for%20'.$title;
	//(!empty($marcinfo['type']) ? $marcinfo['type'] : 'NO TYPE')
	$marcinfo['img'] = _check_url($imgurl) == true ? $imgurl : 'http://placehold.it/200&text='.$marcinfo['type'];
	$marcinfo['largeimg'] = _check_url($largeimgurl) == true ? $largeimgurl : 'http://placehold.it/400&text='.$marcinfo['type'];
	//form_set_error('time', t('-------------------------------------'));
	//_check_url($imgurl, $bib);
	//dsm($marcinfo);
	return $marcinfo;

//return false;
}
/**
 * @description: a helper function to check if the url is valid or not
 */
function _check_url($url, $bib) 
{ 
	$ch = curl_init(); 
	curl_setopt($ch, CURLOPT_URL, $url); 
	curl_setopt($ch, CURLOPT_HEADER, 1); 
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1); 
	$data = curl_exec($ch); 
	$headers = curl_getinfo($ch); 
	curl_close($ch); 
	
	if($headers['http_code'] != 200 || $headers['size_download'] < 200){ return false; }
	else{return true;}
	
	//dsm($bib); dsm($headers['http_code']); 
	//if($bib == '18335' || $bib == '113742' || $bib == '32219') dsm($headers);
	//dsm('--------------------------------------------------------');
	//return $headers['http_code']; 
}
/**
 * local function
 * @description: theme the bib number array
 */
function _theme_eg_item($item) {
	$html = '<div>';
	$html .= (!empty($item['type']) ? '<div class="bibitem-type">'.$item['type'].'</div>' : '');
	$html .= '<div class="bibitem-img"><a href="'.$item['url'].'" target="_blank"><img src="'.$item['img'].'" /></a></div>';
	$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a href="'.$item['url'].'" target="_blank">'.((strlen($item['title']) > 27) ? substr($item['title'],0,24).'...' : $item['title']).'</a></span>' : '');
	//$html .= (!empty($item['author']) ? '<br /><span class="bibitem-author">'.$item['author'].'</span>' : '');
	$html .= (!empty($item['callnumber']) ? '<br /><span class="bibitem-callnumber">'.$item['callnumber'].'</span>' : '');
	
	$html .= '</div>';		
	return $html;
//return $item['type'].' | '.$item['title'].' | '.$item['author'].'<br />'.$item['img'];
}

/**
 * local admin function
 * @description: theme the bib number nodes passed by entity reference field
 */
function _theme_bib_obj($node, $parentId='') {
	$type = $node->field_tpac_local_item_type['und'][0]['value'];
	$imgurl = $node->field_image_url['und'][0]['value'];
	$callnumber = $node->field_call_number['und'][0]['value'];
	$url = $node->field_catalog_url['und'][0]['value'];
	$title = _set_title_length($node->title, 140);
	
	$largeimgurl = $node->field_large_image_url['und'][0]['value'];
	$ebookurl = $node->field_ebook_url['und'][0]['value'];
	$ebookhost = $node->field_ebook_host['und'][0]['value'];
	
	//dsm($GLOBALS['base_url'].'/'.drupal_get_path_alias());
	//dsm($GLOBALS['user']->roles);
	
	$html = '<div class="bib-eachitem-wrapper clearfix '._get_type_color($type).' '._get_filter_css($type).'">';
	$html .= (!empty($type) ? '<div class="bibitem-type">'.$type.'</div>' : '');
	if(in_array('editor-announcement', $GLOBALS['user']->roles) || in_array('editor', $GLOBALS['user']->roles) || in_array('administrator', $GLOBALS['user']->roles)){
		$html .= '<div class="bibitem-group"><a href="/node/'.$node->nid.'/edit?destination='.drupal_get_path_alias().'">edit</a></div>';	
	}
	//$captionlink = '<a href=\"'.$url.'\">'.$title.'</a>';
	$captionlink = '&lt;a target=\'_blank\' href=\''.$url.'\'&gt;'.$title.'&lt;/a&gt;&nbsp;&nbsp;'.(!empty($ebookurl) ? '&lt;a target=\'_blank\' href=\''.$ebookurl.'\'&gt;'.$ebookhost.'&lt;/a&gt;' : '').(!empty($callnumber) ? '&lt;span class=\'bibitem-callnumber\' &gt;'.$callnumber.'&lt;/span&gt;' : '');
	$html .= '<div class="bibitem-img"><a rel="lightbox['.$parentId.']['.$captionlink.']" href="'.$largeimgurl.'"><img src="'.$imgurl.'" /></a></div>';
	//$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a href="'.$item['url'].'">'.((strlen($item['title']) > 27) ? substr($item['title'],0,24).'...' : $item['title']).'</a></span>' : '');
	$html .= (!empty($title) ? '<span class="bibitem-title"><a target="_blank" href="'.$url.'">'.$title.'</a></span>' : '');
	//$html .= (!empty($item['author']) ? '<br /><span class="bibitem-author">'.$item['author'].'</span>' : '');
	$html .= (!empty($callnumber) ? '<span class="bibitem-callnumber">'.$callnumber.'</span>' : '');
	$html .= (!empty($ebookurl) ? '<span class="bibitem-ebook"><a target="_blank" href="'.$ebookurl.'">'.($ebookhost ? $ebookhost : 'ebook title missing').'</a></span>' : '');
	
	$html .= '</div>';		
	return $html;
//return $item['type'].' | '.$item['title'].' | '.$item['author'].'<br />'.$item['img'];
}



/**
 * local function
 * @description: theme the bib number nodes passed by entity reference field, 
 **/
function _theme_bib_item($nodeId, $parentId='') {
	//dsm($nodeId);
	$node = node_load($nodeId);
	$type = $node->field_tpac_local_item_type['und'][0]['value'];
	$imgurl = $node->field_image_url['und'][0]['value'];
	$callnumber = $node->field_call_number['und'][0]['value'];
	$url = $node->field_catalog_url['und'][0]['value'];
	$title = _set_title_length($node->title, 140);
	$largeimgurl = $node->field_large_image_url['und'][0]['value'];
	$ebookurl = $node->field_ebook_url['und'][0]['value'];
	$ebookhost = $node->field_ebook_host['und'][0]['value'];
	
	//dsm($GLOBALS['base_url'].'/'.drupal_get_path_alias());
	//dsm($GLOBALS['user']->roles);
	
	$html = '<div class="bib-eachitem-wrapper clearfix '._get_type_color($type).' '._get_filter_css($type).'">';
	$html .= (!empty($type) ? '<div class="bibitem-type">'.$type.'</div>' : '');
	if(in_array('editor', $GLOBALS['user']->roles) || in_array('administrator', $GLOBALS['user']->roles)){
		$html .= '<div class="bibitem-group"><a href="/node/'.$node->nid.'/edit?destination='.drupal_get_path_alias().'">edit</a></div>';	
	}
	//$captionlink = '<a href=\"'.$url.'\">'.$title.'</a>';
	$captionlink = '&lt;a target=\'_blank\' href=\''.$url.'\'&gt;'.$title.'&lt;/a&gt;&nbsp;&nbsp;'.(!empty($ebookurl) ? '&lt;a target=\'_blank\' href=\''.$ebookurl.'\'&gt;'.$ebookhost.'&lt;/a&gt;' : '');
	$html .= '<div class="bibitem-img"><a rel="lightbox['.$parentId.']['.$captionlink.']" href="'.$largeimgurl.'"><img src="'.$imgurl.'" /></a></div>';
	//$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a href="'.$item['url'].'">'.((strlen($item['title']) > 27) ? substr($item['title'],0,24).'...' : $item['title']).'</a></span>' : '');
	$html .= (!empty($title) ? '<span class="bibitem-title"><a target="_blank" href="'.$url.'">'.$title.'</a></span>' : '');
	//$html .= (!empty($item['author']) ? '<br /><span class="bibitem-author">'.$item['author'].'</span>' : '');
	$html .= (!empty($callnumber) ? '<span class="bibitem-callnumber">'.$callnumber.'</span>' : '');
	$html .= (!empty($ebookurl) ? '<br /><span class="bibitem-ebook"><a target="_blank" href="'.$ebookurl.'">'.($ebookhost ? $ebookhost : 'ebook title missing').'</a></span>' : '');
	
	$html .= '</div>';		
	return $html;
//return $item['type'].' | '.$item['title'].' | '.$item['author'].'<br />'.$item['img'];
}

/**
 * local theme function
 * @description: grab numbers of random items from array
 */
 function _get_rand_items($items, $i = null){
 	
	$ra = $i == null ? shuffle($items) : array_rand($items, $i);
	$si = array();
	foreach($ra as $ri){
		$si[] = $items[$ri];
	}
	return ($si);
 }
 
/**
 * local theme function
 * @description: used for printing bib item, the function used in print.tpl.php
 */
function _theme_bib_obj_print($node, $parentId='') {
	$type = $node->field_tpac_local_item_type['und'][0]['value'];
	$imgurl = $node->field_image_url['und'][0]['value'];
	$callnumber = $node->field_call_number['und'][0]['value'];
	$url = $node->field_catalog_url['und'][0]['value'];
	$title = _set_title_length($node->title, 250);
	$largeimgurl = $node->field_large_image_url['und'][0]['value'];
	$ebookurl = $node->field_ebook_url['und'][0]['value'];
	$ebookhost = $node->field_ebook_host['und'][0]['value'];
	$author = $node->field_bib_author['und'][0]['value'];
	
	//dsm($node);
	
	$html = '<table id="'.$node->nid.'" class="bib-eachitem-print clearfix"><tr>';
	//$html .= (!empty($type) ? '<th class="bibitem-type">'.$type.'</th>' : '');
	//$captionlink = '<a href=\"'.$url.'\">'.$title.'</a>';
	$captionlink = '&lt;a target=\'_blank\' href=\''.$url.'\'&gt;'.$title.'&lt;/a&gt;&nbsp;&nbsp;'.(!empty($ebookurl) ? '&lt;a target=\'_blank\' href=\''.$ebookurl.'\'&gt;'.$ebookhost.'&lt;/a&gt;' : '');
	$html .= '<td class="bibitem-img" valign="top">';
	$html .= '<a rel="lightbox['.$parentId.']['.$captionlink.']" href="'.$largeimgurl.'"><img src="'.$imgurl.'" /></a>';
	$html .= '</td><td class="bibitem-citation"  valign="top">';
	$html .= (!empty($author) ? '<span class="bibitem-author">'.$author.'</span>&nbsp;&nbsp;' : '');
	//$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a href="'.$item['url'].'">'.((strlen($item['title']) > 27) ? substr($item['title'],0,24).'...' : $item['title']).'</a></span>' : '');
	$html .= (!empty($title) ? '<span class="bibitem-title"><a target="_blank" href="'.$url.'">'.$title.'</a></span>' : '');
	$html .= (!empty($callnumber) ? '<br /><span class="bibitem-callnumber-print">'.$callnumber.'</span>' : '');
	$html .= (!empty($ebookurl) ? '<br /><span class="bibitem-ebook">EBooks on '.$ebookhost.'<br />Direct URL: <a target="_blank" href="'.$ebookurl.'">'.str_replace('https://', '', $ebookurl).'</a></span>' : '');
	
	$html .= '</td></tr></table>';		
	return $html;
}
/**
 * local theme function
 * @description: used for printing bib item with proper CITATION(field_bib_citation), the function used in print.tpl.php
 */
function _theme_bib_obj_print_citation($node, $parentId='') {
	
	$imgurl = $node->field_image_url['und'][0]['value'];
	$url = $node->field_catalog_url['und'][0]['value'];
	$largeimgurl = $node->field_large_image_url['und'][0]['value'];
	
	$citation = $node->field_bib_citation['und'][0]['value'];

	$html = '<table id="bib'.$node->nid.'" class="bib-eachitem-print clearfix"><tr>';
	$html .= '<td class="bibitem-img" valign="top">';
	$html .= '<a href="'.$largeimgurl.'"><img src="'.$imgurl.'" /></a>';
	$html .= '</td><td class="bibitem-citation"  valign="top">';
	$html .= !empty($citation) ? $citation._l_edit($node->nid, '[EDIT CITATION]') : _create_citation($node, true)._l_edit($node->nid, '[Edit CITATION]');
	$html .= '</td></tr></table>';		
	return $html;
//return $item['type'].' | '.$item['title'].' | '.$item['author'].'<br />'.$item['img'];
}
function _l_edit($nid, $str){
	return '<div class="citation-edit"><a style="font-size:10px; font-weight:bold; color:#f00" href="/node/'.$nid.'/edit?destination='.current_path().'">'.$str.'</a></div>';
}
function _set_title_length($title, $length=140){
	$title = _rip_str_with($title);
	return (strlen($title) > $length) ? substr($title,0,$length).'...' : $title;
}
// erase string in [ ]
function _rip_str_with($str, $encloser='[]'){	
	switch($encloser){
		case '[]': return preg_replace('/\[.*\]/', '', $str);
		case '()': return preg_replace('/\(.*\)/', '', $str);
	}
}






