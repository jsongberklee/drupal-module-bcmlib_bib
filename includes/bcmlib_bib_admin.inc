<?php
/**
 * local function, dependency : simplehtmldom module
 * @description: 
 */
function _get_feed_items_url($numOfItems='', $type = 'marcxml', $date='', $status='edit'){
//$status='import'; // to grab by created date, but it doesn't mean created date by Berklee
//ex-> http://catalog.berklee.edu/opac/extras/feed/freshmeat/marcxml/biblio/import/3/2014-04-03
	$hosturl = 'https://catalog.berklee.edu'; //$imgparams = '/opac/extras/ac/jacket/medium/r/';
	//$bibparams = '/opac/extras/supercat/retrieve/marcxml/record/'; //for each record
	$bibparams = '/opac/extras/feed/freshmeat/'.$type.'/biblio/'.$status.'/'.$numOfItems.'/'.$date; // for a group of records
	//$bibparams = '/opac/extras/feed/freshmeat/'.$type.'/biblio/'.$status.'/2/'.$date; // for a group of records	
	return $hosturl.$bibparams;
}
function _get_feed_item_url($bibnumber, $type = 'marcxml'){

	return 'https://catalog.berklee.edu/opac/extras/supercat/retrieve/'.$type.'/record/'.$bibnumber; //for each record
}
/**
 * local function
 * @description: 
 *
function _get_feed_items($url, $type='array'){
// $type can pass 'xml' and 'json' as well : to be built later version
	$xml = simplexml_load_file($url);
	foreach($xml->record as $record){			
		//dsm($record->controlfield[0]->__toString());
		//dsm((string)$record->leader); // OK
	}
	//dsm();
}
/**/

/**
 * local admin function
 * @description: theme the bib number array
 */
function _theme_eg_admin_item($item) {
	//dsm($item);
	//$html = '';
	$html = (!empty($item['type']) ? '<div class="bibitem-type">'.$item['type'].'</div>' : '');
	$html .= '<div class="bibitem-img"><a target="_blank" href="'.$item['url'].'"><img src="'.$item['img'].'" /></a></div>';
	//$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a href="'.$item['url'].'">'.((strlen($item['title']) > 27) ? substr($item['title'],0,24).'...' : $item['title']).'</a></span>' : '');
	$html .= (!empty($item['title']) ? '<span class="bibitem-title"><a target="_blank" href="'.$item['url'].'">'.$item['title'].'</a></span><br />' : '');
	$html .= (!empty($item['author']) ? '<span class="bibitem-author">'.$item['author'].'</span><br />' : '');
	$html .= (!empty($item['callnumber']) ? '<span class="bibitem-callnumber">'.$item['callnumber'].'</span>' : '');
	
	//$html .= '';		
	return $html;
//return $item['type'].' | '.$item['title'].' | '.$item['author'].'<br />'.$item['img'];
}
function _get_item_type($leader){
	$code = substr($leader, 6, 1);
	//dsm($code. ' <> ' .$leader);
	switch($code){
		case 'a': return 'Books';
		case 't': return 'Books';
		//case 'a': return '';
		case 'g': return 'Visual Materials';
		case 'k': return 'Visual Materials';
		case 'r': return 'Visual Materials';
		case 'o': return 'Visual Materials';
		case 'p': return 'Mixed Materials';
		case 'e': return 'Maps';
		case 'f': return 'Maps';
		case 'c': return 'Scores';
		case 'd': return 'Scores';
		case 'i': return 'Sound Recordings';
		case 'j': return 'Sound Recordings';
		case 'm': return 'Computer Files';
		default: return 'N/A';
	}
	
}

function _get_type_color($type = ''){
	switch($type){
		case 'Sound Recordings': return 'audio';
		case 'Visual Materials': return 'video';
		case 'Scores': return 'score';
		case 'Books': return 'book';
		default: return 'default';
	}
}

function _get_filter_css($type = ''){
	return trim(str_replace(' ', '', $type));
}
function _get_filterHtml($filterItems, $filterId){
	$html = '<div id="'.$filterId.'" class="button-group radius" style="margin-bottom: 1em;"><button id="'.$filterId.'all" class="button small" data-filter="*">show all</button>';
	foreach($filterItems as $key => $value){$html .='<button id="'.$filterId.$key.'" class="button small audio" data-filter=".'.$key.'">'.$value.'</button>';}	
	$html .='</div>';
	return $html;
}


function _sort_datafield($datafield){
//dsm($datafield);
	$data = array();
	foreach($datafield as $each){
		switch($each['@attributes']['tag']){
			case ('100'): $data['author'] = $each['subfield']; break; // marc 100 preceding over 110
			case ('110'): $data['author'] = isset($data['author']) ? $data['author'] : $each['subfield']; break;
			case ('245'): $data['title'] = $each['subfield']; break;
			case ('090'): $data['callnumber'] = implode(' ', $each['subfield']) ; break; // marc 090 preceding over 050
			case ('050'): $data['callnumber'] = isset($data['callnumber']) ? $data['callnumber'] : implode(' ', $each['subfield']); break;
			case ('856'): $data['ebookurl'] = $each['subfield']; break;
			case ('260'): $data['publisher'] = isset($data['publisher']) ? $data['publisher'] : implode(' ', $each['subfield']); break;
			case ('264'): $data['publisher'] = isset($data['publisher']) ? $data['publisher'] : implode(' ', $each['subfield']); break;
		}
		
		//dsm($datafield['@attributes']['tag']);
	}
	
	//dsm($data);
	return $data;
}

function _get_feed_items($url, $type='array'){
// example feed url : group item
// http://catalog.berklee.edu/opac/extras/feed/freshmeat/marcxml/biblio/edit/25

// example feed url : one item
//https://catalog.berklee.edu/opac/extras/supercat/retrieve/marcxml-full/record/100095

// $type can pass 'xml' and 'json' as well : to be built later version
	$xml = simplexml_load_file($url);
	$json = json_decode(json_encode($xml), 1);
	$sortedArray = array();
	foreach($json['record'] as $record){
		$bib = $record['controlfield'][0];
		$sortedArray[$bib] = _extract_marc_info($record);
	}

	return $sortedArray;
}

function _get_feed_item($url){

	$xml = simplexml_load_file($url);
	$json = json_decode(json_encode($xml), 1);
	$sortedArray = array();
	foreach($json as $record){
		$bib = $record['controlfield'][0];
		$sortedArray[$bib] = _extract_marc_info($record);
	}

	return $sortedArray;
}

function _is_item_exist($bib){

	$query = new EntityFieldQuery;
	$query->entityCondition('entity_type', 'node')->entityCondition('bundle', 'item_bib')->fieldCondition('field_tpac_bib','value', $bib, '=');
	$result = $query->execute();
	
	//dsm($result);
	if(isset($result['node'])){
		foreach($result['node'] as $each){
			return $each->nid;
		}
	}
	else
	{
		return false;
	}

}

function _extract_marc_info($record){
	
	$bib = $record['controlfield'][0];
	$eachBibRecord = array();
	$eachBibRecord['tcn'] = $record['controlfield'][0];
	$eachBibRecord['type'] = _get_item_type($record['leader']);
	$data = _sort_datafield($record['datafield']);
	//dsm($data['title']);
	$eachBibRecord['title'] = _str_truncate((is_array($data['title']) ? implode(' ', $data['title']) : $data['title']), 250, "...");
	$eachBibRecord['callnumber'] = $data['callnumber']; 
	$eachBibRecord['author'] = is_array($data['author']) ? _consolidate_author($data['author']) : $data['author'];
	$eachBibRecord['publisher'] = $data['publisher'];
	
	if($data['ebookurl'][1]){
		if(is_array($data['ebookurl'])){
			$eachBibRecord['ebookhost'] = $data['ebookurl'][0] == 'EBSCOhost' ? 'EBSCO eBook' : $data['ebookurl'][0];
			$long_url = $data['ebookurl'][1];
		}
		else{
			$eachBibRecord['ebookhost'] = 'No_Host_Infomation';
			$long_url = $data['ebookurl'];
		}
		$long_url = is_array($data['ebookurl']) ? $data['ebookurl'][1] : $data['ebookurl'];
		$short_path = 'eb'.$eachBibRecord['tcn'];
		//dsm($eachBibRecord['tcn'].' --> '.$long_url);
		$shorturl = shurly_save_url($long_url, $short_path);
		if($shorturl) $eachBibRecord['ebookurl'] = $GLOBALS['base_url'].'/'.$short_path;
		else drupal_set_message(t('Failed to create short url!'));
	}
	else{ $eachBibRecord['ebookurl'] = ''; $eachBibRecord['ebookhost'] = ''; }
	//$eachBibRecord['ebookurl'] = $data['ebookurl'][1];
	//dsm($eachBibRecord['ebookurl']);
	$eachBibRecord['url'] = 'https://catalog.berklee.edu/eg/opac/record/'.$bib;
	$imgurl = 'https://catalog.berklee.edu/opac/extras/ac/jacket/medium/r/'.$bib;
	$largeimgurl = 'https://catalog.berklee.edu/opac/extras/ac/jacket/large/r/'.$bib;
	//$imgurl = _check_url($imgurl) == 200 ? $imgurl : 'http://placehold.it/200&text=image%20not%20avaiable%20for%20'.$title;
	$isValidUrl = _check_url($imgurl);
	$eachBibRecord['img'] = $isValidUrl == true ? $imgurl : 'http://placehold.it/200&text='.$eachBibRecord['type'];
	$eachBibRecord['largeimg'] = $isValidUrl == true ? $largeimgurl : 'http://placehold.it/400&text='.$eachBibRecord['type'];
	
	return $eachBibRecord;
}
function _consolidate_author($arr){
	if(strpos($arr[0],'(berklee)') !== false){
		unset($arr[0]); $arr = array_values($arr);
	}
	return $arr[0];
}

function _save_to_item_node($item){
	//item_bib, field_bib_author, title, field_tpac_bib, field_call_number, field_image_url, field_catalog_url
	$node = new stdClass(); // We create a new node object
	$node->type = "item_bib"; // Or any other content type you want
	$node->title = $item['title'];
	$node->language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
	//$node->path = array('alias' => 'your node path'); // Setting a node path
	node_object_prepare($node); // Set some default values.
	// Let's add some CCK/Fields API field. This is pretty similar to the body example 
	$node->field_tpac_local_item_type[$node->language][0]['value'] = $item['type'];
	$node->field_tpac_bib[$node->language][0]['value'] = $item['tcn'];
	$node->field_call_number[$node->language][0]['value'] = $item['callnumber'];
	$node->field_image_url[$node->language][0]['value'] = $item['img'];
	$node->field_large_image_url[$node->language][0]['value'] = $item['largeimg'];
	$node->field_catalog_url[$node->language][0]['value'] = $item['url'];
	$node->field_bib_author[$node->language][0]['value'] = $item['author'];
	$node->field_ebook_url[$node->language][0]['value'] = $item['ebookurl'];
	$node->field_ebook_host[$node->language][0]['value'] = $item['ebookhost'];
	$node->field_bib_citation[$node->language][0]['value'] = _create_citation($item);
	// If your custom field has a format, don't forget to define it here
	//$node->field_custom_name[$node->language][0]['format'] = 'This is a custom field value';
	$node = node_submit($node); // Prepare node for a submit
	node_save($node); // After this call we'll get a nid
	return $node;
}

function _create_citation($item, $obj=false){
	if($obj){
		$type = $item->field_tpac_local_item_type['und'][0]['value'];
		$callnumber = $item->field_call_number['und'][0]['value'];
		$url = $item->field_catalog_url['und'][0]['value'];
		$title = _set_title_length($item->title, 250);
		$ebookurl = $item->field_ebook_url['und'][0]['value'];
		$author = $item->field_bib_author['und'][0]['value'];
	
		$html = '<span class="citation-author">'._rip_str_with($author, '()').'</span>.&nbsp;';
		$html .= '<span class="citation-title"><i>'._rip_str_with($title, '[]').'</i></span>&nbsp;';
		$html .= '<span class="citation-publisher">No Publisher Information</span>.&nbsp;';
		$html .= !empty($callnumber) ? '<br /><span class="citation-callnumber">Call # <a href="'.$url.'" target="_blank">'.$callnumber.'</a></span>&nbsp;' : '';
		$html .= !empty($ebookurl) ? '<br /><span class="citation-ebook"><a href="'.$ebookurl.'" target="_blank">'.$ebookurl.'</a></span>&nbsp;' : '';
		
		$node = node_load($item->nid);
		$node->field_bib_citation[$node->language][0]['value'] = $html;
		node_save($node);
	}
	else{
		$html = '<span class="citation-author">'._rip_str_with($item['author'], '()').'</span>.&nbsp;';
		$html .= '<span class="citation-title"><i>'._rip_str_with($item['title'], '[]').'</i></span>&nbsp;';
		$html .= '<span class="citation-publisher">'.$item['publisher'].'</span>.&nbsp;';
		$html .= !empty($item['callnumber']) ? '<br /><span class="citation-callnumber">Call # <a href="'.$item['url'].'" target="_blank">'.$item['callnumber'].'</a></span>&nbsp;' : '';
		$html .= !empty($item['ebookurl']) ? '<br /><span class="citation-ebook"><a href="'.$item['ebookurl'].'" target="_blank">'.$item['ebookurl'].'</a></span>&nbsp;' : '';
	}

	return $html;
}



