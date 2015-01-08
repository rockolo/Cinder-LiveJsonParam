#include "LiveParam.h"
#include "cinder/Filesystem.h"
#include <fstream>

using namespace ci;
using namespace live;

template<>
void Param<vec2>::save( const std::string& name, JsonTree* tree ) const
{
	auto v = JsonTree::makeArray( name );
	v.pushBack( JsonTree( "x", toString( mValue.x ) ) );
	v.pushBack( JsonTree( "y", toString( mValue.y ) ) );
	tree->addChild( v );
}

template<>
void Param<vec3>::save( const std::string& name, JsonTree* tree ) const
{
	auto v = JsonTree::makeArray( name );
	v.pushBack( JsonTree( "x", toString( mValue.x ) ) );
	v.pushBack( JsonTree( "y", toString( mValue.y ) ) );
	v.pushBack( JsonTree( "z", toString( mValue.z ) ) );
	tree->addChild( v );
}

template<>
void Param<vec4>::save( const std::string& name, JsonTree* tree ) const
{
	auto v = JsonTree::makeArray( name );
	v.pushBack( JsonTree( "x", toString( mValue.x ) ) );
	v.pushBack( JsonTree( "y", toString( mValue.y ) ) );
	v.pushBack( JsonTree( "z", toString( mValue.z ) ) );
	v.pushBack( JsonTree( "w", toString( mValue.w ) ) );
	tree->addChild( v );
}

template<>
void Param<quat>::save( const std::string& name, JsonTree* tree ) const
{
	auto v = JsonTree::makeArray( name );
	v.pushBack( JsonTree( "w", toString( mValue.w ) ) );
	v.pushBack( JsonTree( "x", toString( mValue.x ) ) );
	v.pushBack( JsonTree( "y", toString( mValue.y ) ) );
	v.pushBack( JsonTree( "z", toString( mValue.z ) ) );
	tree->addChild( v );
	
}

template<>
void Param<Color>::save( const std::string& name, JsonTree* tree ) const
{
	auto v = JsonTree::makeArray( name );
	v.pushBack( JsonTree( "r", toString( mValue.r ) ) );
	v.pushBack( JsonTree( "g", toString( mValue.g ) ) );
	v.pushBack( JsonTree( "b", toString( mValue.b ) ) );
	tree->addChild( v );
}

template<>
void Param<bool>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	update( iter->getValue<bool>() );
}

template<>
void Param<int>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	update( iter->getValue<int>() );
}

template<>
void Param<float>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	update( iter->getValue<float>() );
}

template<>
void Param<vec2>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	vec2 v;
	v.x = iter->getChild("x").getValue<float>();
	v.y = iter->getChild("y").getValue<float>();
	update( v );
}

template<>
void Param<vec3>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	vec3 v;
	v.x = iter->getChild("x").getValue<float>();
	v.y = iter->getChild("y").getValue<float>();
	v.z = iter->getChild("z").getValue<float>();
	update( v );
}

template<>
void Param<vec4>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	vec4 v;
	v.x = iter->getChild("x").getValue<float>();
	v.y = iter->getChild("y").getValue<float>();
	v.z = iter->getChild("z").getValue<float>();
	v.w = iter->getChild("w").getValue<float>();
	update( v );
}

template<>
void Param<quat>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	quat q;
	q.w = iter->getChild("w").getValue<float>();
	q.x = iter->getChild("x").getValue<float>();
	q.y = iter->getChild("y").getValue<float>();
	q.z = iter->getChild("z").getValue<float>();
	update( q );
}

template<>
void Param<Color>::load( const std::string& name, JsonTree::ConstIter& iter )
{
	Color c;
	c.r = iter->getChild("r").getValue<float>();
	c.g = iter->getChild("g").getValue<float>();
	c.b = iter->getChild("b").getValue<float>();
	update( c );
}

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
