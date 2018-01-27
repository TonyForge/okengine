#include "AssetsBasic.h"

ok::AssetsBasic* ok::AssetsBasic::basic_instance = nullptr;

ok::AssetsBasic & ok::AssetsBasic::instance()
{
	return *basic_instance;
}

ok::sound::SoundAsset::SoundAsset()
{
	_sample_resource = nullptr;
	_stream_resource = "";
}

void ok::sound::SoundAsset::Apply(ok::sound::Sample & sample)
{
	sample.setBuffer(*_sample_resource);
}

void ok::sound::SoundAsset::Apply(ok::sound::Stream & stream)
{
	stream.openFromFile(_stream_resource);
}

bool ok::sound::SoundAsset::IsSample()
{
	return _sample_resource != nullptr;
}

bool ok::sound::SoundAsset::IsStream()
{
	return _stream_resource.size() != 0;
}