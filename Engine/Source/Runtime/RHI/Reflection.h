#pragma once

enum class ShaderVariableType
{
    VOID_,
    BOOL,
    INT,
    FLOAT,
    TEXTURE,
    TEXTURE1D,
    TEXTURE2D,
    TEXTURE3D,
    TEXTURECUBE,
    SAMPLER,
    SAMPLER1D,
    SAMPLER2D,
    SAMPLER3D,
    SAMPLERCUBE,
    UINT,
    UINT8,
    TEXTURE1DARRAY,
    TEXTURE2DARRAY,
    TEXTURE2DMS,
    TEXTURE2DMSARRAY,
    TEXTURECUBEARRAY,
    DOUBLE,
    RWTEXTURE1D,
    RWTEXTURE1DARRAY,
    RWTEXTURE2D,
    RWTEXTURE2DARRAY,
    RWTEXTURE3D,
    RWBUFFER,
    BYTEADDRESS_BUFFER,
    RWBYTEADDRESS_BUFFER,
    STRUCTURED_BUFFER,
    RWSTRUCTURED_BUFFER,
    APPEND_STRUCTURED_BUFFER,
    CONSUME_STRUCTURED_BUFFER,
    MIN8FLOAT,
    MIN10FLOAT,
    MIN16FLOAT,
    MIN12INT,
    MIN16INT,
    MIN16UINT,
    INT16,
    UINT16,
    FLOAT16,
    INT64,
    UINT64,
    MAX
};

enum class ShaderComponentType
{
    UINT32,
    SINT32,
    FLOAT32,
    MAX
};

enum class ShaderVariableClass
{
    SCALAR,
    VECTOR,
    MATRIX_ROWS,
    MATRIX_COLUMNS,
    OBJECT,
    STRUCT,
    MAX
};

enum class ShaderInputType
{
     CBUFFER,
     TBUFFER,
     TEXTURE,
     SAMPLER,
     UAV_RWTYPED,
     STRUCTURED,
     UAV_RWSTRUCTURED,
     BYTEADDRESS,
     UAV_RWBYTEADDRESS,
     UAV_APPEND_STRUCTURED,
     UAV_CONSUME_STRUCTURED,
     UAV_RWSTRUCTURED_WITH_COUNTER,
     RTACCELERATIONSTRUCTURE,
     UAV_FEEDBACKTEXTURE,
     MAX
};


struct SRVariable
{
    std::string         VarName;
    std::string         TypeName;
    ShaderVariableType  Type;
    ShaderVariableClass Class;
    u32                 Offset;   
    u32                 Size;      
    u32                 TextureOffset;    
    u32                 TextureSize;   
    u32                 SamplerOffset;   
    u32                 SamplerSize;    
};

struct SRConstBuffer
{
    std::string Name;
    u32  Size;
    std::vector<SRVariable> Variables;

};

struct SRBoundResource
{
    std::string           Name;         
    ShaderInputType       Type;
    u32                   BindPoint;      
    u32                   BindCount;
    u32                   BindSpace;
};

struct SRSignatureParameter
{
    std::string    SemanticName;    
    u32            SemanticIndex;  
    ShaderComponentType ComponentType;
    u8            ComponentMask;      
};

 
class ShaderReflection
{
public:

    u32      InstructionCount;            // Number of emitted instructions
    u32      TempRegisterCount;           // Number of temporary registers used 

    u32      StaticFlowControlCount;      // Number of static flow control instructions used
    u32      DynamicFlowControlCount;     // Number of dynamic flow control instructions used

    u32      ThreadGroupSizeX;
    u32      ThreadGroupSizeY;
    u32      ThreadGroupSizeZ;

    std::vector<SRConstBuffer>   ConstBuffers;
    std::vector<SRBoundResource> BoundResources;
    std::vector<SRSignatureParameter> InputParameter;
    std::vector<SRSignatureParameter> OutputParameter;
};