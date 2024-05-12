#pragma once

class ShaderManager
{
public:
	static void Add(ShaderResource* shader);
	static void Remove(const std::string& hash);
	static void Destroy();
private:
	static std::map<std::string, ShaderResource*> _ShaderMap;
};


class PipelineManager
{
public:
	static void Add(GraphicPipeline* pso);
	static void Remove(GraphicPipeline* pso);
	static void Destroy();

private:
	static std::set<GraphicPipeline*> _Pipelines;
};

class RootSignatureManager
{
public:
	static void Add(RootSignature* rs);
	static void Remove(RootSignature* rs);
	static void Destroy();

private:
	static std::set<RootSignature*> _RootSignatures;
};