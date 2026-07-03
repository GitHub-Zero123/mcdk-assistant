import mcdk_assistant as mcdk


@mcdk.tool(name="hello", description="返回一个问候")
def hello(args, ctx):
    print("hello plugin called")
    return "hello"
