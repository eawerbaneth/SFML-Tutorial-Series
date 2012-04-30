#version 150

in vec4 pass_Color;

out vec4 out_Color;

void main(void)
{
  out_Color = pass_Color; //FragColor = texture(s_tex, v_tex);
}