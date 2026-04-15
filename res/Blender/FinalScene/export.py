import bpy
import json
import os

def export_mesh(obj):
    if obj.type != 'MESH':
        return None

    depsgraph = bpy.context.evaluated_depsgraph_get()
    eval_obj = obj.evaluated_get(depsgraph)
    mesh = eval_obj.to_mesh()
    mesh.calc_loop_triangles()

    uv_layer = mesh.uv_layers.active.data if mesh.uv_layers else None

    vertices = []    
    indices = []       
    vertex_map = {}   

    def key(pos, uv, normal):
        return (round(pos.x, 6), round(pos.y, 6), round(pos.z, 6),
                round(uv[0], 6), round(uv[1], 6),
                round(normal.x, 6), round(normal.y, 6), round(normal.z, 6))

    for tri in mesh.loop_triangles:
        tri_indices = []

        for loop_idx in tri.loops:
            loop = mesh.loops[loop_idx]
            v = mesh.vertices[loop.vertex_index]
            pos = v.co
            normal = loop.normal 
            if uv_layer:
                uv = uv_layer[loop_idx].uv[:]
            else:
                uv = (0.0, 0.0)

            k = key(pos, uv, normal)

            if k in vertex_map:
                idx = vertex_map[k]
            else:
                idx = len(vertices) // 8
                vertex_map[k] = idx
                vertices.extend([pos.x, pos.y, pos.z, uv[0], uv[1], normal.x, normal.y, normal.z])

            tri_indices.append(idx)

        indices.extend(tri_indices)

    obj.to_mesh_clear()

    return {
        "v": vertices,
        "i": indices
    }

def get_material_textures(obj):
    if not obj.data.materials:
        return {}
    result = {}
    for mat in obj.data.materials:
        if mat and mat.use_nodes:
            nodes = mat.node_tree.nodes
            links = mat.node_tree.links
            principled = next((n for n in nodes if n.type=='BSDF_PRINCIPLED'), None)
            if principled:
                def find_image(name):
                    sock = principled.inputs.get(name)
                    if sock:
                        for l in links:
                            if l.to_socket == sock and l.from_node.type=='TEX_IMAGE' and l.from_node.image:
                                return l.from_node.image.name
                    return None
                for k in ["Base Color","Metallic","Roughness","Normal"]:
                    val = find_image(k)
                    if val: result[k] = val
    return result

def export_scene(filepath):
    data = []
    for obj in bpy.context.scene.objects:
        rot = obj.matrix_world.to_quaternion()
        item = {
            "n": obj.name,
            "t": obj.type,
            "p": list(obj.matrix_world.to_translation()),
            "r": [float(rot.x), float(rot.y), float(rot.z), float(rot.w)],
            "s": list(obj.matrix_world.to_scale())
        }
        m = export_mesh(obj)
        if m: item["m"] = m
        tex = get_material_textures(obj)
        if tex: item["tx"] = tex
        data.append(item)
    with open(filepath,"w") as f:
        json.dump(data, f, separators=(',',':'))

output_path = os.path.join("C:/Users/tpanechou\Documents/GitHub/lyo-2-amiga-s2p4-02/res/Blender","scene_test.json")
export_scene(output_path)