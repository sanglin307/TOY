#include "Private.h"

void RenderPassTest::Init(RenderDevice* device)
{
	_Type = RenderPassType::Test;
}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{

}

void RenderPassTest::AddCommand(MeshCommand* command)
{
	_Commands.push_back(command);
}