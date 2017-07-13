#version 450 core
#pragma debug(on)
#pragma optimize(off)

#pragma include("constants.glsl")
#pragma include("vertex_in_array.glsl")
#pragma include("vertex_out.glsl")
#pragma include("wireframe_geom.glsl")

layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices = 15) out;


void transferVertexData(int i);
bool isFrontFacing(vec3 a, vec3 b, vec3 c);
void emitEdgeQuad(vec3 e0, vec3 e1);


uniform bool isSelected;
uniform float edgeWidth;
uniform float pctExtends;

flat out int isEdge;;

void main(){
	vec3 p0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p1 = gl_in[1].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p2 = gl_in[2].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p3 = gl_in[3].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p4 = gl_in[4].gl_Position.xyz / gl_in[0].gl_Position.w;
	vec3 p5 = gl_in[5].gl_Position.xyz / gl_in[0].gl_Position.w;
	
	if(isSelected && isFrontFacing(p0, p2, p4)){
		if(!isFrontFacing(p0, p1, p2)){
			emitEdgeQuad(p0, p2);
		}
		if(!isFrontFacing(p2, p3, p4)){
			emitEdgeQuad(p2, p4);
		}
		if(!isFrontFacing(p4, p5, p0)){
			emitEdgeQuad(p4, p0);
		}
	}

	isEdge = 0;

	transferVertexData(0);
	EmitVertex();
	
	transferVertexData(2);
	EmitVertex();
	
	transferVertexData(4);
	EmitVertex();
	
}


bool isFrontFacing(vec3 a, vec3 b, vec3 c){
	return (
		(a.x * b.y - b.x * a.y) +
		(b.x * c.y - c.x * b.y) +
		(c.x * a.y - a.x * c.y)
	) > 0;
}

void emitEdgeQuad(vec3 e0, vec3 e1){
	vec2 ext = pctExtends * (e1.xy - e0.xy);
	vec2 v = normalize(e1.xy - e0.xy);
	vec2 n = vec2(-v.y, v.x) * edgeWidth;

	isEdge = 1;

    gl_Position = vec4( e0.xy - ext, e0.z, 1.0 ); EmitVertex();
    gl_Position = vec4( e0.xy - n - ext, e0.z, 1.0 ); EmitVertex();
    gl_Position = vec4( e1.xy + ext, e1.z, 1.0 ); EmitVertex();
    gl_Position = vec4( e1.xy - n + ext, e1.z, 1.0 ); EmitVertex();
	EndPrimitive();
}

void transferVertexData(int i){
	vertex_out.position = vertex_in[i].position;
	vertex_out.normal = vertex_in[i].normal;
	vertex_out.texCoord = vertex_in[i].texCoord;
	vertex_out.eyes = vertex_in[i].eyes;
	vertex_out.color = vertex_in[i].color;
	gl_Position = gl_in[i].gl_Position;	

	for(int j = 0; j < MAX_LIGHT_SOURCES; j++){
		vertex_out.lightDirection[j] = vertex_in[i].lightDirection[j];
	}
}