<script lang="ts">
	import { enhance } from "$app/forms";
	import crashlogo from '$lib/assets/383990724-d3bdb664-5fdc-446d-9361-8037283497e7.png';
</script>

<div class="w-full h-full bg-custom">
<div class="flex flex-col items-center">
	<div class="mr-4">
		<img alt="Logo di CRASH" src={crashlogo} width="120px" />
	</div>
	<div class=" flex-grow">
		<h1 class=" font-bold">C.R.A.S.H - Collision Check System</h1>
	</div>
<h1>Sign in</h1>
<form method="post" use:enhance>
	<label for="username">Username</label><br />
	<input name="username" id="username" class="border-[1px] rounded-md border-black"/><br />
	<label for="password">Password</label><br />
	<input type="password" name="password" id="password" class="border-[1px] rounded-md border-black" /><br />
	<input type="submit" class=" border-[1px] rounded-md bg-green-600 border-green-950 py-1 px-4 mt-4"/>
</form>
</div>
</div>

<style lang="css">
	.bg-custom{
		background-color: rgb(201,201,201);
	}
</style>