<script lang="ts">
	import '../app.css';
	let { children } = $props();
</script>

<div class="flex flex-1 flex-col h-full">
	{@render children()}
</div>
