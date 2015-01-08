#include "LiveParam.h"
#include "cinder/Filesystem.h"
#include <fstream>

#include "cinder/Quaternion.h"

using namespace ci;
using namespace live;

std::unique_ptr<JsonBag> JsonBag::mInstance = nullptr;
std::once_flag JsonBag::mOnceFlag;

JsonBag::JsonBag()
{
	mJsonFilePath = app::getAssetPath("") / "live_params.json";
	
	// Create json file if it doesn't already exist.
	if( ! fs::exists( mJsonFilePath ) ) {
		std::ofstream oStream( mJsonFilePath.string() );
		oStream.close();
	}
	
	wd::watch( mJsonFilePath, [this]( const fs::path &absolutePath )
	{
		this->load();
	} );
}

void JsonBag::save() const
{
	JsonTree doc;
	JsonTree params = JsonTree::makeArray( "params" );
	
	for( const auto& item : mItems ) {
		item.second->save( item.first, &params );
	}
	
	doc.pushBack( params );
	doc.write( writeFile( mJsonFilePath ), JsonTree::WriteOptions() );
}

void JsonBag::load()
{
	if( ! fs::exists( mJsonFilePath ) ) {
		return;
	}
	
	try {
		JsonTree doc( loadFile( mJsonFilePath ) );
		JsonTree params( doc.getChild( "params" ) );
		for( JsonTree::ConstIter item = params.begin(); item != params.end(); ++item ) {
			const auto& name = item->getKey();
			if( mItems.count( name ) ) {
				mItems.at( name )->load( name, item );
			} else {
				CI_LOG_E( "No item named " + name );
			}
		}
	}
	catch( const JsonTree::ExcJsonParserError& )  {
		CI_LOG_E( "Failed to parse json file." );
	}
}

void JsonBag::removeTarget( void *target )
{
	if( ! target )
		return;
	
	for( auto it = mItems.cbegin(); it != mItems.cend(); ++it ) {
		if( it->second->getTarget() == target ) {
			mItems.erase( it );
			return;
		}
	}
	CI_LOG_E( "Target not found." );
}

JsonBag* live::bag()
{
	std::call_once(JsonBag::mOnceFlag,
				   [] {
					   JsonBag::mInstance.reset( new JsonBag );
				   });
	
	return JsonBag::mInstance.get();
}
