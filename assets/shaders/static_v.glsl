#version 330

layout(location = 0) in vec3 position;

uniform mat4 m_Model;
uniform mat4 m_View;
uniform mat4 m_Projection;

void main()
{
	gl_Position = m_Projection * m_View * m_Model * vec4(position, 1.0);
}
