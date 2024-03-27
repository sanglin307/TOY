#pragma once

class ShaderManager
{
public:
	static void Add(ShaderObject* shader);
	static void Remove(const std::string& hash);
	static void Destroy();
private:
	static std::map<std::string, ShaderObject*> _ShaderMap;
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